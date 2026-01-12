#ifndef SPICE_WRAPPERS_H
#define SPICE_WRAPPERS_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include "spice-sdvxio/spice_connection.h"

struct spice_analog_state {
    char *name;
    float value;
};

struct spice_button_state {
    char *name;
    float value;
};

struct spice_light_state {
    char *name;
    float value;
    bool enabled;
};

struct spice_info_launcher {
    char *version;
    char *compile_date;
    char *compile_time;
    char *system_time;
};

uint64_t spice_msg_gen_id();

bool spice_analogs_write(struct spice_connection *con, struct spice_analog_state *states, size_t count);
bool spice_buttons_write(struct spice_connection *con, struct spice_button_state *states, size_t count);
bool spice_coin_set(struct spice_connection *con, int coins);
bool spice_coin_blocker_get(struct spice_connection *con, bool *closed);
bool spice_lights_read(struct spice_connection *con, struct spice_light_state **states, size_t *count);
void spice_lights_free(struct spice_light_state *states, size_t count);

bool spice_control_session_refresh(struct spice_connection *con);
bool spice_info_launcher(struct spice_connection *con, struct spice_info_launcher *info);
void spice_info_launcher_free(struct spice_info_launcher *info);

#endif
