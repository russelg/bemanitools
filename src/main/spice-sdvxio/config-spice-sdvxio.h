#ifndef SPICE_SDVXIO_CONFIG_H
#define SPICE_SDVXIO_CONFIG_H

#include <windows.h>

#include "cconfig/cconfig.h"

#include "sdvxhook-util/config-leds.h"

struct spice_sdvxio_config {
    bool enable_keylight;
    int32_t amp_volume;
    bool analog_enable_sleep;
    float analog_snap_multiplier;
};

bool get_spice_sdvxio_config(
    struct spice_sdvxio_config *config_out,
    struct sdvxhook_config_leds *leds_out);

#endif
