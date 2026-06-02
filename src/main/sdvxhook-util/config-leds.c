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
            "The IO PWM channel you want to assign '%s' to",
            detail->led_name);

        cconfig_util_set_int(
            config,
            detail->config_key,
            detail->config_default,
            led_description);
    }
}

void sdvxhook_config_leds_get(
    struct sdvxhook_config_leds *config_leds, struct cconfig *config)
{
    for (int i = 0; i < lengthof(light_config_details); i++) {
        const struct light_config_detail *detail = &light_config_details[i];

        if (!cconfig_util_get_int(
                config,
                detail->config_key,
                (int *) (((char *) config_leds) + detail->offset),
                detail->config_default)) {
            log_warning(
                "Invalid value for key '%s' specified, fallback "
                "to default '%d'",
                detail->config_key,
                detail->config_default);
        }
    }
}
