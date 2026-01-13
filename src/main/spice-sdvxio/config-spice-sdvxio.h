#ifndef SPICE_SDVXIO_CONFIG_H
#define SPICE_SDVXIO_CONFIG_H

#include <windows.h>

#include "cconfig/cconfig.h"

struct spice_sdvxio_config {
    bool enable_keylight;
    bool service_coin;
    int32_t amp_volume;
    char host[256];
    int32_t port;
    char password[256];
};

bool get_spice_sdvxio_config(struct spice_sdvxio_config *config_out);

#endif