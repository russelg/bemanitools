#define LOG_MODULE "spice-sdvxio"

#include <process.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include <windows.h>

#include "bemanitools/sdvxio.h"
#include "util/defs.h"
#include "util/log.h"
#include "util/thread.h"

#include "sdvxhook-util/config-leds.h"
#include "spice-sdvxio/config-spice-sdvxio.h"
#include "spice-sdvxio/responsive-analog-read.h"
#include "spice-sdvxio/spicesdk.h"
#include "spice-sdvxio/spicesdk_io.h"

#define ANALOG_FIXED_SENSITIVITY 1023.f

static SPICE_SDK_V0 spice;
static HANDLE worker_stop_event;
static HANDLE worker_handle;

static struct spice_sdvxio_config g_config;
static struct sdvxhook_config_leds g_config_leds;

enum gpio_type {
    GPIO_TYPE_SYS,
    GPIO_TYPE_0,
    GPIO_TYPE_1,
};

struct button_map {
    uint32_t sdk_button_id;
    enum gpio_type type;
    uint16_t bitmask;
};

struct analog_map {
    uint32_t sdk_analog_id;
    uint8_t spinner_idx;
    float sensitivity;
    struct responsive_analog_read rar;
};

struct gpio_light_map {
    uint32_t sdk_light_id;
    uint16_t gpio_bit;
};

struct pwm_light_map {
    uint32_t sdk_light_id;
    int pwm_channel;
};

struct config_light_map {
    uint32_t sdk_light_id;
    size_t config_leds_offset;
};

/* CoinMech is handled solely by handle_coins to avoid override conflicts. */
static const struct button_map g_button_maps[] = {
    {SDVX_Button_Test,      GPIO_TYPE_SYS, SDVX_IO_IN_GPIO_SYS_TEST},
    {SDVX_Button_Service,   GPIO_TYPE_SYS, SDVX_IO_IN_GPIO_SYS_SERVICE},
    {SDVX_Button_BT_A,      GPIO_TYPE_0,   SDVX_IO_IN_GPIO_0_A},
    {SDVX_Button_BT_B,      GPIO_TYPE_0,   SDVX_IO_IN_GPIO_0_B},
    {SDVX_Button_BT_C,      GPIO_TYPE_0,   SDVX_IO_IN_GPIO_0_C},
    {SDVX_Button_BT_D,      GPIO_TYPE_1,   SDVX_IO_IN_GPIO_1_D},
    {SDVX_Button_FX_L,      GPIO_TYPE_1,   SDVX_IO_IN_GPIO_1_FX_L},
    {SDVX_Button_FX_R,      GPIO_TYPE_1,   SDVX_IO_IN_GPIO_1_FX_R},
    {SDVX_Button_Start,     GPIO_TYPE_0,   SDVX_IO_IN_GPIO_0_START},
    {SDVX_Button_Headphone, GPIO_TYPE_0,   SDVX_IO_IN_GPIO_0_HEADPHONE},
};

static struct analog_map g_analog_maps[] = {
    {SDVX_Analog_VOL_L, 0, ANALOG_FIXED_SENSITIVITY, {0}},
    {SDVX_Analog_VOL_R, 1, ANALOG_FIXED_SENSITIVITY, {0}},
};

static const struct gpio_light_map g_gpio_light_maps[] = {
    {SDVX_Light_BT_A,        SDVX_IO_OUT_GPIO_A},
    {SDVX_Light_BT_B,        SDVX_IO_OUT_GPIO_B},
    {SDVX_Light_BT_C,        SDVX_IO_OUT_GPIO_C},
    {SDVX_Light_BT_D,        SDVX_IO_OUT_GPIO_D},
    {SDVX_Light_FX_L,        SDVX_IO_OUT_GPIO_FX_L},
    {SDVX_Light_FX_R,        SDVX_IO_OUT_GPIO_FX_R},
    {SDVX_Light_START,       SDVX_IO_OUT_GPIO_START},
    {SDVX_Light_GENERATOR_B, SDVX_IO_OUT_GPIO_GENERATOR_B},
};

/* Generator R/G are hardcoded PWM channels not in the shared LED config. */
static const struct pwm_light_map g_pwm_light_maps[] = {
    {SDVX_Light_GENERATOR_R, GENERATOR_R},
    {SDVX_Light_GENERATOR_G, GENERATOR_G},
};

static const struct config_light_map g_config_light_maps[] = {
    {SDVX_Light_TITLE_AVG_R,
     offsetof(struct sdvxhook_config_leds, title_avg_r_pin)},
    {SDVX_Light_TITLE_AVG_G,
     offsetof(struct sdvxhook_config_leds, title_avg_g_pin)},
    {SDVX_Light_TITLE_AVG_B,
     offsetof(struct sdvxhook_config_leds, title_avg_b_pin)},
    {SDVX_Light_UPPER_LEFT_SPEAKER_AVG_R,
     offsetof(struct sdvxhook_config_leds, upper_left_speaker_avg_r_pin)},
    {SDVX_Light_UPPER_LEFT_SPEAKER_AVG_G,
     offsetof(struct sdvxhook_config_leds, upper_left_speaker_avg_g_pin)},
    {SDVX_Light_UPPER_LEFT_SPEAKER_AVG_B,
     offsetof(struct sdvxhook_config_leds, upper_left_speaker_avg_b_pin)},
    {SDVX_Light_UPPER_RIGHT_SPEAKER_AVG_R,
     offsetof(struct sdvxhook_config_leds, upper_right_speaker_avg_r_pin)},
    {SDVX_Light_UPPER_RIGHT_SPEAKER_AVG_G,
     offsetof(struct sdvxhook_config_leds, upper_right_speaker_avg_g_pin)},
    {SDVX_Light_UPPER_RIGHT_SPEAKER_AVG_B,
     offsetof(struct sdvxhook_config_leds, upper_right_speaker_avg_b_pin)},
    {SDVX_Light_LEFT_WING_AVG_R,
     offsetof(struct sdvxhook_config_leds, left_wing_avg_r_pin)},
    {SDVX_Light_LEFT_WING_AVG_G,
     offsetof(struct sdvxhook_config_leds, left_wing_avg_g_pin)},
    {SDVX_Light_LEFT_WING_AVG_B,
     offsetof(struct sdvxhook_config_leds, left_wing_avg_b_pin)},
    {SDVX_Light_RIGHT_WING_AVG_R,
     offsetof(struct sdvxhook_config_leds, right_wing_avg_r_pin)},
    {SDVX_Light_RIGHT_WING_AVG_G,
     offsetof(struct sdvxhook_config_leds, right_wing_avg_g_pin)},
    {SDVX_Light_RIGHT_WING_AVG_B,
     offsetof(struct sdvxhook_config_leds, right_wing_avg_b_pin)},
    {SDVX_Light_LOWER_LEFT_SPEAKER_AVG_R,
     offsetof(struct sdvxhook_config_leds, lower_left_speaker_avg_r_pin)},
    {SDVX_Light_LOWER_LEFT_SPEAKER_AVG_G,
     offsetof(struct sdvxhook_config_leds, lower_left_speaker_avg_g_pin)},
    {SDVX_Light_LOWER_LEFT_SPEAKER_AVG_B,
     offsetof(struct sdvxhook_config_leds, lower_left_speaker_avg_b_pin)},
    {SDVX_Light_LOWER_RIGHT_SPEAKER_AVG_R,
     offsetof(struct sdvxhook_config_leds, lower_right_speaker_avg_r_pin)},
    {SDVX_Light_LOWER_RIGHT_SPEAKER_AVG_G,
     offsetof(struct sdvxhook_config_leds, lower_right_speaker_avg_g_pin)},
    {SDVX_Light_LOWER_RIGHT_SPEAKER_AVG_B,
     offsetof(struct sdvxhook_config_leds, lower_right_speaker_avg_b_pin)},
    {SDVX_Light_CONTROL_PANEL_AVG_R,
     offsetof(struct sdvxhook_config_leds, control_panel_avg_r_pin)},
    {SDVX_Light_CONTROL_PANEL_AVG_G,
     offsetof(struct sdvxhook_config_leds, control_panel_avg_g_pin)},
    {SDVX_Light_CONTROL_PANEL_AVG_B,
     offsetof(struct sdvxhook_config_leds, control_panel_avg_b_pin)},
    {SDVX_Light_WOOFER_AVG_R,
     offsetof(struct sdvxhook_config_leds, woofer_avg_r_pin)},
    {SDVX_Light_WOOFER_AVG_G,
     offsetof(struct sdvxhook_config_leds, woofer_avg_g_pin)},
    {SDVX_Light_WOOFER_AVG_B,
     offsetof(struct sdvxhook_config_leds, woofer_avg_b_pin)},
    {SDVX_Light_V_UNIT_AVG_R,
     offsetof(struct sdvxhook_config_leds, v_unit_avg_r_pin)},
    {SDVX_Light_V_UNIT_AVG_G,
     offsetof(struct sdvxhook_config_leds, v_unit_avg_g_pin)},
    {SDVX_Light_V_UNIT_AVG_B,
     offsetof(struct sdvxhook_config_leds, v_unit_avg_b_pin)},
    {SDVX_Light_ICCR_R,
     offsetof(struct sdvxhook_config_leds, ic_card_reader_r_pin)},
    {SDVX_Light_ICCR_G,
     offsetof(struct sdvxhook_config_leds, ic_card_reader_g_pin)},
    {SDVX_Light_ICCR_B,
     offsetof(struct sdvxhook_config_leds, ic_card_reader_b_pin)},
};

static bool check_key(uint16_t input, size_t idx_in)
{
    return (input >> idx_in) & 1;
}

static uint16_t check_assign_gpio(uint16_t input, size_t idx_in, size_t gpio_out)
{
    if (check_key(input, idx_in)) {
        return 1 << gpio_out;
    }

    return 0;
}

static uint16_t assign_gpio(bool active, size_t gpio_out)
{
    if (active) {
        return 1 << gpio_out;
    }

    return 0;
}

static uint8_t rescale_light_value(float value)
{
    if (value <= 0.03f) {
        return 0;
    }

    if (value >= 1.0f) {
        return 255;
    }

    return (uint8_t) (((value - 0.03f) / (1.0f - 0.03f)) * 255.f);
}

static void handle_buttons(uint8_t sys, uint16_t gpio0, uint16_t gpio1)
{
    size_t num_buttons = lengthof(g_button_maps);

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

        bool pressed = check_key(current_gpio, g_button_maps[i].bitmask);
        SPICE_SDK_STATUS_CODE result = spice.set_button(
            g_button_maps[i].sdk_button_id, pressed, pressed ? 1.0f : 0.0f);

        if (result != SPICE_SDK_STATUS_SUCCESS) {
            static bool warned = false;

            if (!warned) {
                warned = true;
                log_warning(
                    "set_button failed for id %u (status %d); "
                    "further failures suppressed",
                    g_button_maps[i].sdk_button_id,
                    result);
            }
        }
    }
}

static void handle_analogs(void)
{
    size_t num_analogs = lengthof(g_analog_maps);

    for (size_t i = 0; i < num_analogs; i++) {
        responsive_analog_read_update(
            &g_analog_maps[i].rar,
            sdvx_io_get_spinner_pos(g_analog_maps[i].spinner_idx));

        float value =
            responsive_analog_read_get_value(&g_analog_maps[i].rar) /
            g_analog_maps[i].sensitivity;

        SPICE_SDK_STATUS_CODE result =
            spice.set_analog(g_analog_maps[i].sdk_analog_id, true, value);

        if (result != SPICE_SDK_STATUS_SUCCESS) {
            static bool warned = false;

            if (!warned) {
                warned = true;
                log_warning(
                    "set_analog failed for id %u (status %d); "
                    "further failures suppressed",
                    g_analog_maps[i].sdk_analog_id,
                    result);
            }
        }
    }
}

static void write_keylights(uint16_t gpio0, uint16_t gpio1, uint16_t *gpio_lights)
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

static void handle_lights(uint16_t *gpio_lights)
{
    float value;
    SPICE_SDK_STATUS_CODE result;

    for (size_t i = 0; i < lengthof(g_gpio_light_maps); i++) {
        result = spice.get_light(g_gpio_light_maps[i].sdk_light_id, &value);

        if (result != SPICE_SDK_STATUS_SUCCESS) {
            static bool warned = false;

            if (!warned) {
                warned = true;
                log_warning(
                    "get_light failed for gpio light id %u (status %d); "
                    "further failures suppressed",
                    g_gpio_light_maps[i].sdk_light_id,
                    result);
            }

            continue;
        }

        *gpio_lights |= assign_gpio(value > 0, g_gpio_light_maps[i].gpio_bit);
    }

    for (size_t i = 0; i < lengthof(g_pwm_light_maps); i++) {
        result = spice.get_light(g_pwm_light_maps[i].sdk_light_id, &value);

        if (result != SPICE_SDK_STATUS_SUCCESS) {
            static bool warned = false;

            if (!warned) {
                warned = true;
                log_warning(
                    "get_light failed for pwm light id %u (status %d); "
                    "further failures suppressed",
                    g_pwm_light_maps[i].sdk_light_id,
                    result);
            }

            continue;
        }

        sdvx_io_set_pwm_light(
            g_pwm_light_maps[i].pwm_channel, rescale_light_value(value));
    }

    for (size_t i = 0; i < lengthof(g_config_light_maps); i++) {
        result = spice.get_light(g_config_light_maps[i].sdk_light_id, &value);

        if (result != SPICE_SDK_STATUS_SUCCESS) {
            static bool warned = false;

            if (!warned) {
                warned = true;
                log_warning(
                    "get_light failed for config light id %u (status %d); "
                    "further failures suppressed",
                    g_config_light_maps[i].sdk_light_id,
                    result);
            }

            continue;
        }

        int *pins =
            (int *) (((char *) &g_config_leds) +
                     g_config_light_maps[i].config_leds_offset);

        for (int k = 0;
             k < SDVXHOOK_CONFIG_LEDS_MAX_PINS && pins[k] != PIN_END;
             k++) {
            sdvx_io_set_pwm_light(pins[k], rescale_light_value(value));
        }
    }
}

static uint64_t g_coin_button_release_at;
static uint32_t g_coin_button_id;

static void handle_coins(void)
{
    static bool initialized = false;

    if (!initialized) {
        initialized = true;
        log_info("unlocking coin mech");

        sdvx_io_set_coin_blocker(false);
        g_coin_button_id = g_config.service_coin ? SDVX_Button_Service
                                                  : SDVX_Button_CoinMech;
    }

    static int coinstock_last = 0;
    int coinstock = sdvx_io_get_coins();

    if (coinstock != coinstock_last) {
        coinstock_last = coinstock;
        log_info("coins %d, pulsing button %u", coinstock, g_coin_button_id);
        g_coin_button_release_at = GetTickCount64() + 150;
    }

    bool pulse = GetTickCount64() < g_coin_button_release_at;

    spice.set_button(g_coin_button_id, pulse, pulse ? 1.0f : 0.0f);
}

static unsigned __stdcall worker_thread(void *arg)
{
    log_info("poll loop starting");

    while (WaitForSingleObject(worker_stop_event, 1) == WAIT_TIMEOUT) {
        sdvx_io_read_input();

        uint8_t sys    = sdvx_io_get_input_gpio_sys();
        uint16_t gpio0 = sdvx_io_get_input_gpio(0);
        uint16_t gpio1 = sdvx_io_get_input_gpio(1);

        handle_coins();
        handle_buttons(sys, gpio0, gpio1);
        handle_analogs();

        uint16_t gpio_lights = 0;

        if (g_config.enable_keylight) {
            write_keylights(gpio0, gpio1, &gpio_lights);
        }

        handle_lights(&gpio_lights);

        sdvx_io_set_gpio_lights(gpio_lights);
        sdvx_io_write_output();
    }

    log_info("poll loop stopped");

    return 0;
}

#define IMPLEMENT_SPICE_SINK(name, level)                           \
    static void name(const char *module, const char *fmt, ...)      \
    {                                                               \
        char buf[512];                                              \
        va_list args;                                               \
        va_start(args, fmt);                                        \
        vsnprintf(buf, sizeof(buf), fmt, args);                     \
        va_end(args);                                               \
        spice.log(level, module, buf);                              \
    }

IMPLEMENT_SPICE_SINK(spice_log_misc,    SPICE_SDK_LOG_LEVEL_MISC)
IMPLEMENT_SPICE_SINK(spice_log_info,    SPICE_SDK_LOG_LEVEL_INFO)
IMPLEMENT_SPICE_SINK(spice_log_warning, SPICE_SDK_LOG_LEVEL_WARNING)
IMPLEMENT_SPICE_SINK(spice_log_fatal,   SPICE_SDK_LOG_LEVEL_FATAL)

static void __cdecl destroy_callback(void)
{
    log_info("shutting down");

    if (worker_stop_event) {
        SetEvent(worker_stop_event);
    }

    if (worker_handle) {
        WaitForSingleObject(worker_handle, INFINITE);
        CloseHandle(worker_handle);
        worker_handle = NULL;
    }

    if (worker_stop_event) {
        CloseHandle(worker_stop_event);
        worker_stop_event = NULL;
    }

    sdvx_io_set_amp_volume(96, 96, 96);
    Sleep(1000);
    sdvx_io_fini();
}

SPICE_SDK_ENTRY_POINT
spice_sdk_entry_point(spice_sdk_init_func *init)
{
    memset(&spice, 0, sizeof(spice));
    spice.size = sizeof(spice);

    if (init(0, destroy_callback, &spice) != SPICE_SDK_STATUS_SUCCESS) {
        /* Logging is not yet available; nothing we can do here. */
        return 0;
    }

    if (!spice.log || !spice.get_game_info || !spice.get_light ||
        !spice.set_button || !spice.set_analog) {
        if (spice.log) {
            spice.log(
                SPICE_SDK_LOG_LEVEL_WARNING,
                "spice-sdvxio",
                "required SDK functions missing; update spice2x");
        }

        return 0;
    }

    log_to_external(
        spice_log_misc, spice_log_info, spice_log_warning, spice_log_fatal);

    SPICE_SDK_GAME_INFO gi = {0};
    if (spice.get_game_info(&gi) == SPICE_SDK_STATUS_SUCCESS) {
        log_info("game: %s", gi.name);
    } else {
        log_warning("get_game_info failed");
    }

    if (!get_spice_sdvxio_config(&g_config, &g_config_leds)) {
        log_warning(
            "config load failed; add --config spice-sdvxio.conf to spice "
            "command line to load config, using defaults");
    }

    sdvx_io_set_loggers(
        log_impl_misc, log_impl_info, log_impl_warning, log_impl_fatal);

    if (!sdvx_io_init(crt_thread_create, crt_thread_join, crt_thread_destroy)) {
        spice.log(
            SPICE_SDK_LOG_LEVEL_WARNING,
            "spice-sdvxio",
            "sdvx_io_init failed; check that sdvxio.dll is present");
        return 0;
    }

    log_misc("amp volume: %d", g_config.amp_volume);
    sdvx_io_set_amp_volume(
        g_config.amp_volume, g_config.amp_volume, g_config.amp_volume);

    for (size_t i = 0; i < lengthof(g_analog_maps);
         i++) {
        responsive_analog_read_init(
            &g_analog_maps[i].rar,
            g_config.analog_enable_sleep,
            g_config.analog_snap_multiplier);
    }

    worker_stop_event = CreateEventA(NULL, TRUE, FALSE, NULL);
    worker_handle =
        (HANDLE) _beginthreadex(NULL, 0, worker_thread, NULL, 0, NULL);

    if (!worker_stop_event || !worker_handle) {
        spice.log(
            SPICE_SDK_LOG_LEVEL_WARNING,
            "spice-sdvxio",
            "failed to create worker thread");
        destroy_callback();
        return 0;
    }

    log_info("loaded successfully");

    return 1;
}
