#ifndef SDVXHOOK2_VALK_TAPELED_H
#define SDVXHOOK2_VALK_TAPELED_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

enum tapeled_color_pick_algorithm {
    TAPE_LED_USE_NONE = 0,
    TAPE_LED_USE_FIRST = 1,
    TAPE_LED_USE_MIDDLE = 2,
    TAPE_LED_USE_LAST = 3,
    TAPE_LED_USE_AVERAGE = 4,
};

extern enum tapeled_color_pick_algorithm TAPE_LED_ALGORITHM;

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} tapeled_rgb_t;

bool tapeled_is_enabled(void);
tapeled_rgb_t tapeled_pick_color(const uint8_t *data, size_t data_size);

#endif
