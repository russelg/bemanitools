#ifndef SDVXHOOK2_VALK_CONFIG_IO_H
#define SDVXHOOK2_VALK_CONFIG_IO_H

#include <windows.h>
#include <stdbool.h>

#include "cconfig/cconfig.h"

struct sdvxhook2_valk_config_io {
    bool disable_card_reader_emu;
    bool disable_bio2_emu;
    bool disable_poll_limiter;
    bool force_headphones;
    bool disable_file_hooks;
    bool disable_power_hooks;
    bool disable_nvapi_hooks;
    char card_reader_port[64];
};

void sdvxhook2_valk_config_io_init(struct cconfig *config);

void sdvxhook2_valk_config_io_get(
    struct sdvxhook2_valk_config_io *config_io, struct cconfig *config);

#endif
