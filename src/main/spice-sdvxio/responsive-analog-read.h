/*
 * responsive-analog-read.h
 * C port of ResponsiveAnalogRead Arduino library
 */

#ifndef RESPONSIVE_ANALOG_READ_H
#define RESPONSIVE_ANALOG_READ_H

#include <stdbool.h>

struct responsive_analog_read {
    int analog_resolution;
    float snap_multiplier;
    bool sleep_enable;
    float activity_threshold;
    bool edge_snap_enable;

    float smooth_value;
    float error_ema;
    bool sleeping;

    int raw_value;
    int responsive_value;
    int prev_responsive_value;
    bool responsive_value_has_changed;
};

void responsive_analog_read_init(struct responsive_analog_read *rar, bool sleep_enable, float snap_multiplier);
void responsive_analog_read_update(struct responsive_analog_read *rar, int raw_value);

inline int responsive_analog_read_get_value(struct responsive_analog_read *rar) { return rar->responsive_value; }
inline int responsive_analog_read_get_raw_value(struct responsive_analog_read *rar) { return rar->raw_value; }
inline bool responsive_analog_read_has_changed(struct responsive_analog_read *rar) { return rar->responsive_value_has_changed; }
inline bool responsive_analog_read_is_sleeping(struct responsive_analog_read *rar) { return rar->sleeping; }

void responsive_analog_read_set_snap_multiplier(struct responsive_analog_read *rar, float new_multiplier);
inline void responsive_analog_read_enable_sleep(struct responsive_analog_read *rar) { rar->sleep_enable = true; }
inline void responsive_analog_read_disable_sleep(struct responsive_analog_read *rar) { rar->sleep_enable = false; }
inline void responsive_analog_read_enable_edge_snap(struct responsive_analog_read *rar) { rar->edge_snap_enable = true; }
inline void responsive_analog_read_disable_edge_snap(struct responsive_analog_read *rar) { rar->edge_snap_enable = false; }
inline void responsive_analog_read_set_activity_threshold(struct responsive_analog_read *rar, float new_threshold) { rar->activity_threshold = new_threshold; }
inline void responsive_analog_read_set_analog_resolution(struct responsive_analog_read *rar, int resolution) { rar->analog_resolution = resolution; }

#endif
