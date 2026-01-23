#include <stddef.h>

#include "cconfig/cconfig-util.h"

#include "sdvxhook2-valk/config-leds.h"

#include "util/log.h"
#include "util/str.h"

void sdvxhook2_valk_config_leds_init(struct cconfig *config)
{
    char led_description[80];
    int32_t default_pin;

    for (int i = 0; i < lengthof(light_config_details); i++) {
        const struct light_config_detail *detail = &light_config_details[i];

        str_format(
            led_description,
            sizeof(led_description),
            "The IO PWM channel you want to assign '%s' to",
            detail->led_name);

        default_pin = detail->config_default;
        cconfig_util_set_int_array(
            config,
            detail->config_key,
            &default_pin,
            1,
            NULL,
            led_description);
    }
}

void sdvxhook2_valk_config_leds_get(
    struct sdvxhook2_valk_config_leds *config_leds, struct cconfig *config)
{
    for (int i = 0; i < lengthof(light_config_details); i++) {
        const struct light_config_detail *detail = &light_config_details[i];
        int32_t default_pins[SDVXHOOK2_VALK_CONFIG_LEDS_PIN_MAX];

        default_pins[0] = detail->config_default;
        for (int j = 1; j < lengthof(default_pins); j++) {
            default_pins[j] = PIN_END;
        }

        if (!cconfig_util_get_int_array(
                config,
                detail->config_key,
                (int32_t *) (((char *) config_leds) + detail->offset),
                lengthof(default_pins),
                default_pins,
                NULL)) {
            log_warning(
                "Invalid value for key '%s' specified, fallback to defaults",
                detail->config_key);
        }
    }
}
