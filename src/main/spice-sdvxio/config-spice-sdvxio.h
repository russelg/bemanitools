#ifndef SPICE_SDVXIO_CONFIG_H
#define SPICE_SDVXIO_CONFIG_H

#include <windows.h>

#include "cconfig/cconfig.h"

#include "sdvxhook-util/config-leds.h"

struct spice_sdvxio_config {
    bool enable_keylight;
    bool service_coin;
    int32_t amp_volume;
    char host[256];
    int32_t port;
    char password[256];
    bool analog_enable_sleep;
    float analog_snap_multiplier;
};

bool get_spice_sdvxio_config(
    struct spice_sdvxio_config *config_out,
    struct sdvxhook_config_leds *leds_out);

#endif