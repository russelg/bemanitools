#ifndef IIDXHOOK9_AIO_IOB2_H
#define IIDXHOOK9_AIO_IOB2_H

#include "sdvxhook2-valk/config-leds.h"

// hooks functions needed to emulate valk cab IO
void aio_iob2_hook_init(bool disable_poll_limiter, bool force_headphones, struct sdvxhook2_valk_config_leds *config_leds);

#endif
