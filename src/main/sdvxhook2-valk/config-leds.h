#ifndef SDVXHOOK2_VALK_CONFIG_LEDS_H
#define SDVXHOOK2_VALK_CONFIG_LEDS_H

#include <windows.h>
#include <stdbool.h>

#include "cconfig/cconfig.h"

enum tape_led_light {
    TITLE_AVG_R,
    TITLE_AVG_G,
    TITLE_AVG_B,
    UPPER_LEFT_SPEAKER_AVG_R,
    UPPER_LEFT_SPEAKER_AVG_G,
    UPPER_LEFT_SPEAKER_AVG_B,
    UPPER_RIGHT_SPEAKER_AVG_R,
    UPPER_RIGHT_SPEAKER_AVG_G,
    UPPER_RIGHT_SPEAKER_AVG_B,
    LEFT_WING_AVG_R,
    LEFT_WING_AVG_G,
    LEFT_WING_AVG_B,
    RIGHT_WING_AVG_R,
    RIGHT_WING_AVG_G,
    RIGHT_WING_AVG_B,
    LOWER_LEFT_SPEAKER_AVG_R,
    LOWER_LEFT_SPEAKER_AVG_G,
    LOWER_LEFT_SPEAKER_AVG_B,
    LOWER_RIGHT_SPEAKER_AVG_R,
    LOWER_RIGHT_SPEAKER_AVG_G,
    LOWER_RIGHT_SPEAKER_AVG_B,
    CONTROL_PANEL_AVG_R,
    CONTROL_PANEL_AVG_G,
    CONTROL_PANEL_AVG_B,
    WOOFER_AVG_R,
    WOOFER_AVG_G,
    WOOFER_AVG_B,
    V_UNIT_AVG_R,
    V_UNIT_AVG_G,
    V_UNIT_AVG_B,
};

enum pwm_light {
    PIN_END = -1,
    WING_LEFT_UP_R,
    WING_LEFT_UP_G,
    WING_LEFT_UP_B,
    WING_RIGHT_UP_R,
    WING_RIGHT_UP_G,
    WING_RIGHT_UP_B,
    WING_LEFT_LOW_R,
    WING_LEFT_LOW_G,
    WING_LEFT_LOW_B,
    WING_RIGHT_LOW_R,
    WING_RIGHT_LOW_G,
    WING_RIGHT_LOW_B,
    WOOFER_R,
    WOOFER_G,
    WOOFER_B,
    CONTROLLER_R,
    CONTROLLER_G,
    CONTROLLER_B,
    GENERATOR_R,
    GENERATOR_G,
};

struct sdvxhook2_valk_config_leds {
    int title_avg_r_pin;
    int title_avg_g_pin;
    int title_avg_b_pin;
    int upper_left_speaker_avg_r_pin;
    int upper_left_speaker_avg_g_pin;
    int upper_left_speaker_avg_b_pin;
    int upper_right_speaker_avg_r_pin;
    int upper_right_speaker_avg_g_pin;
    int upper_right_speaker_avg_b_pin;
    int left_wing_avg_r_pin;
    int left_wing_avg_g_pin;
    int left_wing_avg_b_pin;
    int right_wing_avg_r_pin;
    int right_wing_avg_g_pin;
    int right_wing_avg_b_pin;
    int lower_left_speaker_avg_r_pin;
    int lower_left_speaker_avg_g_pin;
    int lower_left_speaker_avg_b_pin;
    int lower_right_speaker_avg_r_pin;
    int lower_right_speaker_avg_g_pin;
    int lower_right_speaker_avg_b_pin;
    int control_panel_avg_r_pin;
    int control_panel_avg_g_pin;
    int control_panel_avg_b_pin;
    int woofer_avg_r_pin;
    int woofer_avg_g_pin;
    int woofer_avg_b_pin;
    int v_unit_avg_r_pin;
    int v_unit_avg_g_pin;
    int v_unit_avg_b_pin;
};

void sdvxhook2_valk_config_leds_init(struct cconfig *config);

void sdvxhook2_valk_config_leds_get(
    struct sdvxhook2_valk_config_leds *config_io, struct cconfig *config);

#endif // SDVXHOOK2_VALK_CONFIG_LEDS_H
