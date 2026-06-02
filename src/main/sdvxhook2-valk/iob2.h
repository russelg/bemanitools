#ifndef SDVXHOOK2_VALK_IOB2_H
#define SDVXHOOK2_VALK_IOB2_H

#include "sdvxhook-util/config-leds.h"

// hooks functions needed to emulate valk cab IO
void aio_iob2_hook_init(bool disable_poll_limiter, bool force_headphones, struct sdvxhook_config_leds *config_leds);

#endif
