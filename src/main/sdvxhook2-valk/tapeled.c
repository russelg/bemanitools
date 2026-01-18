#include "sdvxhook2-valk/tapeled.h"

enum tapeled_color_pick_algorithm TAPE_LED_ALGORITHM = TAPE_LED_USE_MIDDLE;

bool tapeled_is_enabled(void)
{
    return (TAPE_LED_ALGORITHM != TAPE_LED_USE_NONE);
}

tapeled_rgb_t tapeled_pick_color(const uint8_t *data, size_t data_size)
{
    tapeled_rgb_t result = {0, 0, 0};

    if (TAPE_LED_ALGORITHM == TAPE_LED_USE_AVERAGE) {
        size_t avg_ri = 0;
        size_t avg_gi = 0;
        size_t avg_bi = 0;

        for (size_t i = 0; i < data_size; i++) {
            const uint8_t *color = &data[i * 3];
            avg_ri += color[0];
            avg_gi += color[1];
            avg_bi += color[2];
        }

        result.r = (uint8_t) (avg_ri / data_size);
        result.g = (uint8_t) (avg_gi / data_size);
        result.b = (uint8_t) (avg_bi / data_size);
    } else if (
        TAPE_LED_ALGORITHM == TAPE_LED_USE_FIRST ||
        TAPE_LED_ALGORITHM == TAPE_LED_USE_MIDDLE ||
        TAPE_LED_ALGORITHM == TAPE_LED_USE_LAST) {
        const uint8_t *color;

        switch (TAPE_LED_ALGORITHM) {
            case TAPE_LED_USE_FIRST:
                color = &data[0];
                break;
            case TAPE_LED_USE_LAST:
                color = &data[(data_size - 1) * 3];
                break;
            case TAPE_LED_USE_MIDDLE:
            default:
                color = &data[(data_size / 2) * 3];
                break;
        }

        result.r = color[0];
        result.g = color[1];
        result.b = color[2];
    }

    return result;
}