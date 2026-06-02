#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <windows.h>

#include "bemanitools/sdvxio.h"
#include "util/log.h"
#include "util/thread.h"

#include "sdvxhook-util/config-leds.h"
#include "spice-sdvxio/config-spice-sdvxio.h"
#include "spice-sdvxio/responsive-analog-read.h"
#include "spice-sdvxio/spice_wrappers.h"

#define ANALOG_FIXED_SENSITIVITY 1023.f

enum gpio_type {
    GPIO_TYPE_SYS,
    GPIO_TYPE_0,
    GPIO_TYPE_1,
};

struct button_map {
    const char *name;
    enum gpio_type type;
    uint16_t bitmask;
};

struct analog_map {
    const char *name;
    uint8_t spinner_idx;
    float sensitivity;
    struct responsive_analog_read rar;
};

static const struct button_map g_button_maps[] = {
    {"Test", GPIO_TYPE_SYS, SDVX_IO_IN_GPIO_SYS_TEST},
    {"Service", GPIO_TYPE_SYS, SDVX_IO_IN_GPIO_SYS_SERVICE},
    {"Coin Mech", GPIO_TYPE_SYS, SDVX_IO_IN_GPIO_SYS_COIN},
    {"BT-A", GPIO_TYPE_0, SDVX_IO_IN_GPIO_0_A},
    {"BT-B", GPIO_TYPE_0, SDVX_IO_IN_GPIO_0_B},
    {"BT-C", GPIO_TYPE_0, SDVX_IO_IN_GPIO_0_C},
    {"BT-D", GPIO_TYPE_1, SDVX_IO_IN_GPIO_1_D},
    {"FX-L", GPIO_TYPE_1, SDVX_IO_IN_GPIO_1_FX_L},
    {"FX-R", GPIO_TYPE_1, SDVX_IO_IN_GPIO_1_FX_R},
    {"Start", GPIO_TYPE_0, SDVX_IO_IN_GPIO_0_START},
    {"Headphone", GPIO_TYPE_0, SDVX_IO_IN_GPIO_0_HEADPHONE},
};

static struct analog_map g_analog_maps[] = {
    {"VOL-L", 0, ANALOG_FIXED_SENSITIVITY, {0}},
    {"VOL-R", 1, ANALOG_FIXED_SENSITIVITY, {0}},
};

struct light_map_gpio {
    const char *name;
    uint16_t gpio_bit;
};

struct light_map_rgb {
    const char *name;
    int pwm_channel;
};

static const struct light_map_gpio g_gpio_maps[] = {
    {"BT-A", SDVX_IO_OUT_GPIO_A},
    {"BT-B", SDVX_IO_OUT_GPIO_B},
    {"BT-C", SDVX_IO_OUT_GPIO_C},
    {"BT-D", SDVX_IO_OUT_GPIO_D},
    {"FX-L", SDVX_IO_OUT_GPIO_FX_L},
    {"FX-R", SDVX_IO_OUT_GPIO_FX_R},
    {"Start", SDVX_IO_OUT_GPIO_START},
    {"Generator B", SDVX_IO_OUT_GPIO_GENERATOR_B},
};

// Generator R/G are hardcoded PWM lights not in the shared LED config.
// refer to src/main/sdvxio-bio2/sdvxio.c::sdvx_io_write_output for pin layout.
static const struct light_map_rgb g_generator_rgb_maps[] = {
    {"Generator R", GENERATOR_R},
    {"Generator G", GENERATOR_G},
};

static struct sdvxhook_config_leds g_config_leds;

bool check_key(uint16_t input, size_t idx_in)
{
    if ((input >> idx_in) & 1) {
        return true;
    }

    return false;
}

uint16_t check_assign_gpio(uint16_t input, size_t idx_in, size_t gpio_out)
{
    if (check_key(input, idx_in)) {
        return 1 << gpio_out;
    }

    return 0;
}

uint16_t assign_gpio(bool active, size_t gpio_out)
{
    if (active) {
        return 1 << gpio_out;
    }

    return 0;
}

uint8_t rescale_light_value(float value)
{
    if (value <= 0.03f) {
        return 0;
    }

    if (value >= 1.0f) {
        return 255;
    }

    return (uint8_t) (((value - 0.03f) / (1.0f - 0.03f)) * 255.f);
}

static uint64_t g_service_override_timeout = 0;

void handle_buttons(
    struct spice_connection *connection,
    uint8_t sys,
    uint16_t gpio0,
    uint16_t gpio1)
{
    size_t num_buttons = sizeof(g_button_maps) / sizeof(g_button_maps[0]);
    struct spice_button_state *button_states =
        calloc(num_buttons, sizeof(struct spice_button_state));

    for (size_t i = 0; i < num_buttons; i++) {
        uint16_t current_gpio = 0;
        switch (g_button_maps[i].type) {
            case GPIO_TYPE_SYS:
                current_gpio = sys;
                break;
            case GPIO_TYPE_0:
                current_gpio = gpio0;
                break;
            case GPIO_TYPE_1:
                current_gpio = gpio1;
                break;
        }

        button_states[i].name = (char *) g_button_maps[i].name;
        button_states[i].value =
            check_key(current_gpio, g_button_maps[i].bitmask) ? 1.0f : 0.0f;

        // handle service being pressed for coins
        if (!strcmp(button_states[i].name, "Service") &&
            GetTickCount64() < g_service_override_timeout) {
            button_states[i].value = 1.0f;
        }
    }

    spice_buttons_write(connection, button_states, num_buttons);
    free(button_states);
}

void handle_analogs(struct spice_connection *connection)
{
    size_t num_analogs = sizeof(g_analog_maps) / sizeof(g_analog_maps[0]);
    struct spice_analog_state *analog_states =
        calloc(num_analogs, sizeof(struct spice_analog_state));
    for (size_t i = 0; i < num_analogs; i++) {
        responsive_analog_read_update(
            &g_analog_maps[i].rar,
            sdvx_io_get_spinner_pos(g_analog_maps[i].spinner_idx));

        analog_states[i].name = (char *) g_analog_maps[i].name;
        analog_states[i].value =
            responsive_analog_read_get_value(&g_analog_maps[i].rar) /
            g_analog_maps[i].sensitivity;
    }

    spice_analogs_write(connection, analog_states, num_analogs);
    free(analog_states);
}

void write_keylights(uint16_t gpio0, uint16_t gpio1, uint16_t *gpio_lights)
{
    *gpio_lights |= check_assign_gpio(
        gpio0, SDVX_IO_IN_GPIO_0_START, SDVX_IO_OUT_GPIO_START);
    *gpio_lights |=
        check_assign_gpio(gpio0, SDVX_IO_IN_GPIO_0_A, SDVX_IO_OUT_GPIO_A);
    *gpio_lights |=
        check_assign_gpio(gpio0, SDVX_IO_IN_GPIO_0_B, SDVX_IO_OUT_GPIO_B);
    *gpio_lights |=
        check_assign_gpio(gpio0, SDVX_IO_IN_GPIO_0_C, SDVX_IO_OUT_GPIO_C);
    *gpio_lights |=
        check_assign_gpio(gpio1, SDVX_IO_IN_GPIO_1_D, SDVX_IO_OUT_GPIO_D);
    *gpio_lights |=
        check_assign_gpio(gpio1, SDVX_IO_IN_GPIO_1_FX_L, SDVX_IO_OUT_GPIO_FX_L);
    *gpio_lights |=
        check_assign_gpio(gpio1, SDVX_IO_IN_GPIO_1_FX_R, SDVX_IO_OUT_GPIO_FX_R);
}

void handle_lights(struct spice_connection *connection, uint16_t *gpio_lights)
{
    struct spice_light_state *light_states;
    size_t light_count;

    if (spice_lights_read(connection, &light_states, &light_count)) {
        for (size_t i = 0; i < light_count; i++) {
            struct spice_light_state *light = &light_states[i];

            size_t gpio_count = sizeof(g_gpio_maps) / sizeof(g_gpio_maps[0]);

            for (size_t j = 0; j < gpio_count; j++) {
                if (!strcmp(light->name, g_gpio_maps[j].name)) {
                    *gpio_lights |=
                        assign_gpio(light->value > 0, g_gpio_maps[j].gpio_bit);
                }
            }

            size_t detail_count = lengthof(light_config_details);

            for (size_t j = 0; j < detail_count; j++) {
                const struct light_config_detail *detail =
                    &light_config_details[j];

                if (!strcmp(light->name, detail->led_name)) {
                    int pin =
                        *(int *) (((char *) &g_config_leds) + detail->offset);

                    if (pin != PIN_END) {
                        sdvx_io_set_pwm_light(
                            pin, rescale_light_value(light->value));
                    }
                }
            }

            size_t gen_count =
                sizeof(g_generator_rgb_maps) / sizeof(g_generator_rgb_maps[0]);

            for (size_t j = 0; j < gen_count; j++) {
                if (!strcmp(light->name, g_generator_rgb_maps[j].name)) {
                    sdvx_io_set_pwm_light(
                        g_generator_rgb_maps[j].pwm_channel,
                        rescale_light_value(light->value));
                }
            }
        }

        spice_lights_free(light_states, light_count);
    }
}

void handle_coins(struct spice_connection *connection, bool use_service_button)
{
    static bool initialized = false;

    if (!initialized) {
        initialized = true;
        log_info("unlocking coin mech");

        // enable coins
        sdvx_io_set_coin_blocker(false);
    }

    // get coinstock
    static int coinstock_last = 0;
    int coinstock = sdvx_io_get_coins();

    // update coinstock
    if (coinstock != coinstock_last) {
        coinstock_last = coinstock;
        log_info("coins set to %d", coinstock);

        if (use_service_button) {
            log_info("adding coin by pressing the service button");
            g_service_override_timeout = GetTickCount64() + 500;
        } else {
            log_info("setting coins to %d via spice_coin_set", coinstock);
            spice_coin_set(connection, coinstock);
        }
    }

    static uint64_t time_last = 0;
    if (GetTickCount64() >= time_last + 500) {
        time_last = GetTickCount64();

        // get new state
        bool closed;
        static bool last_state = false;
        if (spice_coin_blocker_get(connection, &closed)) {
            // check for change
            if (last_state != closed) {
                last_state = closed;

                // update coin blocker
                sdvx_io_set_coin_blocker(closed);
            }
        }
    }
}

int main(int argc, char **argv)
{
    log_to_writer(log_writer_stdout, NULL);

    struct spice_sdvxio_config config;

    if (!get_spice_sdvxio_config(&config, &g_config_leds)) {
        exit(EXIT_FAILURE);
    }

    sdvx_io_set_loggers(
        log_impl_misc, log_impl_info, log_impl_warning, log_impl_fatal);

    if (!sdvx_io_init(crt_thread_create, crt_thread_join, crt_thread_destroy)) {
        log_warning("Initializing sdvxio failed");
        return -1;
    }

    sdvx_io_set_amp_volume(
        config.amp_volume, config.amp_volume, config.amp_volume);

    // create connection
    struct spice_connection *connection =
        spice_connection_open(config.host, config.port, config.password);
    spice_control_session_refresh(connection);

    // test connection
    struct spice_info_launcher test_info;
    if (!spice_info_launcher(connection, &test_info)) {
        log_warning("unable to connect, waiting for server...");
        while (!spice_info_launcher(connection, &test_info)) {
            Sleep(500);

            sdvx_io_read_input();
            uint8_t sys = sdvx_io_get_input_gpio_sys();
            if (check_key(sys, SDVX_IO_IN_GPIO_SYS_TEST) &&
                check_key(sys, SDVX_IO_IN_GPIO_SYS_SERVICE)) {
                spice_info_launcher_free(&test_info);
                goto END;
            }
        }
    }
    spice_info_launcher_free(&test_info);

    log_info("kfchicken detected");

    bool loop = true;

    uint8_t sys;
    uint16_t gpio0;
    uint16_t gpio1;

    for (size_t i = 0; i < sizeof(g_analog_maps) / sizeof(g_analog_maps[0]);
         i++) {
        responsive_analog_read_init(
            &g_analog_maps[i].rar,
            config.analog_enable_sleep,
            config.analog_snap_multiplier);
    }

    log_info("spice init succeeded, beginning poll loop");

    while (loop) {
        // check connnection
        if (!spice_connection_check(connection)) {
            log_warning("connection lost, waiting for server...");
            while (!spice_connection_check(connection)) {
                Sleep(500);

                sdvx_io_read_input();
                sys = sdvx_io_get_input_gpio_sys();
                if (check_key(sys, SDVX_IO_IN_GPIO_SYS_TEST) &&
                    check_key(sys, SDVX_IO_IN_GPIO_SYS_SERVICE)) {
                    goto END;
                }
            }
            log_info("reconnected");
        }

        sdvx_io_read_input();

        sys = sdvx_io_get_input_gpio_sys();
        gpio0 = sdvx_io_get_input_gpio(0);
        gpio1 = sdvx_io_get_input_gpio(1);

        // handle coins
        handle_coins(connection, config.service_coin);

        // write buttons
        handle_buttons(connection, sys, gpio0, gpio1);

        // write analogs
        handle_analogs(connection);

        uint16_t gpio_lights = 0;

        if (config.enable_keylight) {
            write_keylights(gpio0, gpio1, &gpio_lights);
        }

        // read lights
        handle_lights(connection, &gpio_lights);

        // write lights to GPIO (staging)
        sdvx_io_set_gpio_lights(gpio_lights);

        // flush output to IO
        sdvx_io_write_output();

        // exit loop if TEST+SERVICE is pressed
        if (check_key(sys, SDVX_IO_IN_GPIO_SYS_TEST) &&
            check_key(sys, SDVX_IO_IN_GPIO_SYS_SERVICE)) {
            loop = false;
        }

        // avoid banging
        Sleep(1);
    }

END:
    sdvx_io_set_amp_volume(96, 96, 96);
    Sleep(1000);

    sdvx_io_fini();
    spice_connection_close(connection);

    return 0;
}
