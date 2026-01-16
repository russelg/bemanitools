#include "responsive-analog-read.h"
#include <math.h>
#include <stdlib.h>

static float snap_curve(float x)
{
    float y = 1.0f / (x + 1.0f);
    y = (1.0f - y) * 2.0f;
    if (y > 1.0f) {
        return 1.0f;
    }
    return y;
}

static int get_responsive_value(struct responsive_analog_read *rar, int new_value)
{
    // if sleep and edge snap are enabled and the new value is very close to an edge, drag it a little closer to the edges
    if (rar->sleep_enable && rar->edge_snap_enable) {
        if (new_value < rar->activity_threshold) {
            new_value = (new_value * 2) - (int)rar->activity_threshold;
        } else if (new_value > rar->analog_resolution - rar->activity_threshold) {
            new_value = (new_value * 2) - rar->analog_resolution + (int)rar->activity_threshold;
        }
    }

    // get difference between new input value and current smooth value
    unsigned int diff = abs(new_value - (int)rar->smooth_value);

    // measure the difference between the new value and current value
    // and use another exponential moving average to work out what
    // the current margin of error is
    rar->error_ema += ((new_value - rar->smooth_value) - rar->error_ema) * 0.4f;

    // if sleep has been enabled, sleep when the amount of error is below the activity threshold
    if (rar->sleep_enable) {
        // recalculate sleeping status
        rar->sleeping = fabsf(rar->error_ema) < rar->activity_threshold;
    }

    // if we're allowed to sleep, and we're sleeping
    // then don't update responsiveValue this loop
    // just output the existing responsiveValue
    if (rar->sleep_enable && rar->sleeping) {
        return (int)rar->smooth_value;
    }

    // use a 'snap curve' function
    float snap = snap_curve(diff * rar->snap_multiplier);

    // when sleep is enabled, the emphasis is stopping on a responsiveValue quickly
    if (rar->sleep_enable) {
        snap *= 0.5f + 0.5f; // This was snap *= 0.5 + 0.5 in C++, which is snap *= 1.0. 
                             // Keeping it as is from the original source.
    }

    // calculate the exponential moving average based on the snap
    rar->smooth_value += (new_value - rar->smooth_value) * snap;

    // ensure output is in bounds
    if (rar->smooth_value < 0.0f) {
        rar->smooth_value = 0.0f;
    } else if (rar->smooth_value > rar->analog_resolution - 1) {
        rar->smooth_value = (float)rar->analog_resolution - 1.0f;
    }

    // expected output is an integer
    return (int)rar->smooth_value;
}

void responsive_analog_read_init(struct responsive_analog_read *rar, bool sleep_enable, float snap_multiplier)
{
    rar->analog_resolution = 1024;
    rar->sleep_enable = sleep_enable;
    rar->activity_threshold = 4.0f;
    rar->edge_snap_enable = true;
    rar->smooth_value = 0.0f;
    rar->error_ema = 0.0f;
    rar->sleeping = false;
    rar->raw_value = 0;
    rar->responsive_value = 0;
    rar->prev_responsive_value = 0;
    rar->responsive_value_has_changed = false;
    
    responsive_analog_read_set_snap_multiplier(rar, snap_multiplier);
}

void responsive_analog_read_update(struct responsive_analog_read *rar, int raw_value)
{
    rar->raw_value = raw_value;
    rar->prev_responsive_value = rar->responsive_value;
    rar->responsive_value = get_responsive_value(rar, raw_value);
    rar->responsive_value_has_changed = rar->responsive_value != rar->prev_responsive_value;
}

void responsive_analog_read_set_snap_multiplier(struct responsive_analog_read *rar, float new_multiplier)
{
    if (new_multiplier > 1.0f) {
        new_multiplier = 1.0f;
    }
    if (new_multiplier < 0.0f) {
        new_multiplier = 0.0f;
    }
    rar->snap_multiplier = new_multiplier;
}
