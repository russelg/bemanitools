#include <stddef.h>

#include "cconfig/cconfig-util.h"

#include "sdvxhook-util/config-leds.h"

#include "util/log.h"
#include "util/str.h"

void sdvxhook_config_leds_init(struct cconfig *config)
{
    char led_description[80];

    for (int i = 0; i < lengthof(light_config_details); i++) {
        const struct light_config_detail *detail = &light_config_details[i];

        str_format(
            led_description,
            sizeof(led_description),
            "The IO PWM channel(s) you want to assign '%s' to",
            detail->led_name);

        int32_t default_pins[1] = {detail->config_default};

        cconfig_util_set_int_array(
            config,
            detail->config_key,
            default_pins,
            1,
            " ",
            led_description);
    }
}

void sdvxhook_config_leds_get(
    struct sdvxhook_config_leds *config_leds, struct cconfig *config)
{
    for (int i = 0; i < lengthof(light_config_details); i++) {
        const struct light_config_detail *detail = &light_config_details[i];

        int32_t default_pins[1] = {detail->config_default};

        if (!cconfig_util_get_int_array(
                config,
                detail->config_key,
                (int32_t *) (((char *) config_leds) + detail->offset),
                SDVXHOOK_CONFIG_LEDS_MAX_PINS,
                default_pins,
                1,
                " \t")) {
            log_warning(
                "Invalid value for key '%s' specified, fallback "
                "to default '%d'",
                detail->config_key,
                detail->config_default);
        }
    }
}
