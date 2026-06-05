#ifndef SDVXHOOK_CONFIG_LEDS_H
#define SDVXHOOK_CONFIG_LEDS_H

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
    IC_CARD_READER_R,
    IC_CARD_READER_G,
    IC_CARD_READER_B,
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

#define SDVXHOOK_CONFIG_LEDS_MAX_PINS 20

struct sdvxhook_config_leds {
    int title_avg_r_pin[SDVXHOOK_CONFIG_LEDS_MAX_PINS];
    int title_avg_g_pin[SDVXHOOK_CONFIG_LEDS_MAX_PINS];
    int title_avg_b_pin[SDVXHOOK_CONFIG_LEDS_MAX_PINS];
    int upper_left_speaker_avg_r_pin[SDVXHOOK_CONFIG_LEDS_MAX_PINS];
    int upper_left_speaker_avg_g_pin[SDVXHOOK_CONFIG_LEDS_MAX_PINS];
    int upper_left_speaker_avg_b_pin[SDVXHOOK_CONFIG_LEDS_MAX_PINS];
    int upper_right_speaker_avg_r_pin[SDVXHOOK_CONFIG_LEDS_MAX_PINS];
    int upper_right_speaker_avg_g_pin[SDVXHOOK_CONFIG_LEDS_MAX_PINS];
    int upper_right_speaker_avg_b_pin[SDVXHOOK_CONFIG_LEDS_MAX_PINS];
    int left_wing_avg_r_pin[SDVXHOOK_CONFIG_LEDS_MAX_PINS];
    int left_wing_avg_g_pin[SDVXHOOK_CONFIG_LEDS_MAX_PINS];
    int left_wing_avg_b_pin[SDVXHOOK_CONFIG_LEDS_MAX_PINS];
    int right_wing_avg_r_pin[SDVXHOOK_CONFIG_LEDS_MAX_PINS];
    int right_wing_avg_g_pin[SDVXHOOK_CONFIG_LEDS_MAX_PINS];
    int right_wing_avg_b_pin[SDVXHOOK_CONFIG_LEDS_MAX_PINS];
    int lower_left_speaker_avg_r_pin[SDVXHOOK_CONFIG_LEDS_MAX_PINS];
    int lower_left_speaker_avg_g_pin[SDVXHOOK_CONFIG_LEDS_MAX_PINS];
    int lower_left_speaker_avg_b_pin[SDVXHOOK_CONFIG_LEDS_MAX_PINS];
    int lower_right_speaker_avg_r_pin[SDVXHOOK_CONFIG_LEDS_MAX_PINS];
    int lower_right_speaker_avg_g_pin[SDVXHOOK_CONFIG_LEDS_MAX_PINS];
    int lower_right_speaker_avg_b_pin[SDVXHOOK_CONFIG_LEDS_MAX_PINS];
    int control_panel_avg_r_pin[SDVXHOOK_CONFIG_LEDS_MAX_PINS];
    int control_panel_avg_g_pin[SDVXHOOK_CONFIG_LEDS_MAX_PINS];
    int control_panel_avg_b_pin[SDVXHOOK_CONFIG_LEDS_MAX_PINS];
    int woofer_avg_r_pin[SDVXHOOK_CONFIG_LEDS_MAX_PINS];
    int woofer_avg_g_pin[SDVXHOOK_CONFIG_LEDS_MAX_PINS];
    int woofer_avg_b_pin[SDVXHOOK_CONFIG_LEDS_MAX_PINS];
    int v_unit_avg_r_pin[SDVXHOOK_CONFIG_LEDS_MAX_PINS];
    int v_unit_avg_g_pin[SDVXHOOK_CONFIG_LEDS_MAX_PINS];
    int v_unit_avg_b_pin[SDVXHOOK_CONFIG_LEDS_MAX_PINS];
    int ic_card_reader_r_pin[SDVXHOOK_CONFIG_LEDS_MAX_PINS];
    int ic_card_reader_g_pin[SDVXHOOK_CONFIG_LEDS_MAX_PINS];
    int ic_card_reader_b_pin[SDVXHOOK_CONFIG_LEDS_MAX_PINS];
};

#define SDVXHOOK_CONFIG_LEDS_TITLE_AVG_R_PIN_KEY "leds.title_avg_r_pin"
#define SDVXHOOK_CONFIG_LEDS_TITLE_AVG_G_PIN_KEY "leds.title_avg_g_pin"
#define SDVXHOOK_CONFIG_LEDS_TITLE_AVG_B_PIN_KEY "leds.title_avg_b_pin"
#define SDVXHOOK_CONFIG_LEDS_UPPER_LEFT_SPEAKER_AVG_R_PIN_KEY "leds.upper_left_speaker_avg_r_pin"
#define SDVXHOOK_CONFIG_LEDS_UPPER_LEFT_SPEAKER_AVG_G_PIN_KEY "leds.upper_left_speaker_avg_g_pin"
#define SDVXHOOK_CONFIG_LEDS_UPPER_LEFT_SPEAKER_AVG_B_PIN_KEY "leds.upper_left_speaker_avg_b_pin"
#define SDVXHOOK_CONFIG_LEDS_UPPER_RIGHT_SPEAKER_AVG_R_PIN_KEY "leds.upper_right_speaker_avg_r_pin"
#define SDVXHOOK_CONFIG_LEDS_UPPER_RIGHT_SPEAKER_AVG_G_PIN_KEY "leds.upper_right_speaker_avg_g_pin"
#define SDVXHOOK_CONFIG_LEDS_UPPER_RIGHT_SPEAKER_AVG_B_PIN_KEY "leds.upper_right_speaker_avg_b_pin"
#define SDVXHOOK_CONFIG_LEDS_LEFT_WING_AVG_R_PIN_KEY "leds.left_wing_avg_r_pin"
#define SDVXHOOK_CONFIG_LEDS_LEFT_WING_AVG_G_PIN_KEY "leds.left_wing_avg_g_pin"
#define SDVXHOOK_CONFIG_LEDS_LEFT_WING_AVG_B_PIN_KEY "leds.left_wing_avg_b_pin"
#define SDVXHOOK_CONFIG_LEDS_RIGHT_WING_AVG_R_PIN_KEY "leds.right_wing_avg_r_pin"
#define SDVXHOOK_CONFIG_LEDS_RIGHT_WING_AVG_G_PIN_KEY "leds.right_wing_avg_g_pin"
#define SDVXHOOK_CONFIG_LEDS_RIGHT_WING_AVG_B_PIN_KEY "leds.right_wing_avg_b_pin"
#define SDVXHOOK_CONFIG_LEDS_LOWER_LEFT_SPEAKER_AVG_R_PIN_KEY "leds.lower_left_speaker_avg_r_pin"
#define SDVXHOOK_CONFIG_LEDS_LOWER_LEFT_SPEAKER_AVG_G_PIN_KEY "leds.lower_left_speaker_avg_g_pin"
#define SDVXHOOK_CONFIG_LEDS_LOWER_LEFT_SPEAKER_AVG_B_PIN_KEY "leds.lower_left_speaker_avg_b_pin"
#define SDVXHOOK_CONFIG_LEDS_LOWER_RIGHT_SPEAKER_AVG_R_PIN_KEY "leds.lower_right_speaker_avg_r_pin"
#define SDVXHOOK_CONFIG_LEDS_LOWER_RIGHT_SPEAKER_AVG_G_PIN_KEY "leds.lower_right_speaker_avg_g_pin"
#define SDVXHOOK_CONFIG_LEDS_LOWER_RIGHT_SPEAKER_AVG_B_PIN_KEY "leds.lower_right_speaker_avg_b_pin"
#define SDVXHOOK_CONFIG_LEDS_CONTROL_PANEL_AVG_R_PIN_KEY "leds.control_panel_avg_r_pin"
#define SDVXHOOK_CONFIG_LEDS_CONTROL_PANEL_AVG_G_PIN_KEY "leds.control_panel_avg_g_pin"
#define SDVXHOOK_CONFIG_LEDS_CONTROL_PANEL_AVG_B_PIN_KEY "leds.control_panel_avg_b_pin"
#define SDVXHOOK_CONFIG_LEDS_WOOFER_AVG_R_PIN_KEY "leds.woofer_avg_r_pin"
#define SDVXHOOK_CONFIG_LEDS_WOOFER_AVG_G_PIN_KEY "leds.woofer_avg_g_pin"
#define SDVXHOOK_CONFIG_LEDS_WOOFER_AVG_B_PIN_KEY "leds.woofer_avg_b_pin"
#define SDVXHOOK_CONFIG_LEDS_V_UNIT_AVG_R_PIN_KEY "leds.v_unit_avg_r_pin"
#define SDVXHOOK_CONFIG_LEDS_V_UNIT_AVG_G_PIN_KEY "leds.v_unit_avg_g_pin"
#define SDVXHOOK_CONFIG_LEDS_V_UNIT_AVG_B_PIN_KEY "leds.v_unit_avg_b_pin"
#define SDVXHOOK_CONFIG_LEDS_IC_CARD_READER_R_PIN_KEY "leds.ic_card_reader_r_pin"
#define SDVXHOOK_CONFIG_LEDS_IC_CARD_READER_G_PIN_KEY "leds.ic_card_reader_g_pin"
#define SDVXHOOK_CONFIG_LEDS_IC_CARD_READER_B_PIN_KEY "leds.ic_card_reader_b_pin"

#define SDVXHOOK_CONFIG_LEDS_DEFAULT_TITLE_AVG_R_PIN_VALUE (-1)
#define SDVXHOOK_CONFIG_LEDS_DEFAULT_TITLE_AVG_G_PIN_VALUE (-1)
#define SDVXHOOK_CONFIG_LEDS_DEFAULT_TITLE_AVG_B_PIN_VALUE (-1)
#define SDVXHOOK_CONFIG_LEDS_DEFAULT_UPPER_LEFT_SPEAKER_AVG_R_PIN_VALUE (-1)
#define SDVXHOOK_CONFIG_LEDS_DEFAULT_UPPER_LEFT_SPEAKER_AVG_G_PIN_VALUE (-1)
#define SDVXHOOK_CONFIG_LEDS_DEFAULT_UPPER_LEFT_SPEAKER_AVG_B_PIN_VALUE (-1)
#define SDVXHOOK_CONFIG_LEDS_DEFAULT_UPPER_RIGHT_SPEAKER_AVG_R_PIN_VALUE (-1)
#define SDVXHOOK_CONFIG_LEDS_DEFAULT_UPPER_RIGHT_SPEAKER_AVG_G_PIN_VALUE (-1)
#define SDVXHOOK_CONFIG_LEDS_DEFAULT_UPPER_RIGHT_SPEAKER_AVG_B_PIN_VALUE (-1)
#define SDVXHOOK_CONFIG_LEDS_DEFAULT_LEFT_WING_AVG_R_PIN_VALUE (-1)
#define SDVXHOOK_CONFIG_LEDS_DEFAULT_LEFT_WING_AVG_G_PIN_VALUE (-1)
#define SDVXHOOK_CONFIG_LEDS_DEFAULT_LEFT_WING_AVG_B_PIN_VALUE (-1)
#define SDVXHOOK_CONFIG_LEDS_DEFAULT_RIGHT_WING_AVG_R_PIN_VALUE (-1)
#define SDVXHOOK_CONFIG_LEDS_DEFAULT_RIGHT_WING_AVG_G_PIN_VALUE (-1)
#define SDVXHOOK_CONFIG_LEDS_DEFAULT_RIGHT_WING_AVG_B_PIN_VALUE (-1)
#define SDVXHOOK_CONFIG_LEDS_DEFAULT_LOWER_LEFT_SPEAKER_AVG_R_PIN_VALUE (-1)
#define SDVXHOOK_CONFIG_LEDS_DEFAULT_LOWER_LEFT_SPEAKER_AVG_G_PIN_VALUE (-1)
#define SDVXHOOK_CONFIG_LEDS_DEFAULT_LOWER_LEFT_SPEAKER_AVG_B_PIN_VALUE (-1)
#define SDVXHOOK_CONFIG_LEDS_DEFAULT_LOWER_RIGHT_SPEAKER_AVG_R_PIN_VALUE (-1)
#define SDVXHOOK_CONFIG_LEDS_DEFAULT_LOWER_RIGHT_SPEAKER_AVG_G_PIN_VALUE (-1)
#define SDVXHOOK_CONFIG_LEDS_DEFAULT_LOWER_RIGHT_SPEAKER_AVG_B_PIN_VALUE (-1)
#define SDVXHOOK_CONFIG_LEDS_DEFAULT_CONTROL_PANEL_AVG_R_PIN_VALUE (-1)
#define SDVXHOOK_CONFIG_LEDS_DEFAULT_CONTROL_PANEL_AVG_G_PIN_VALUE (-1)
#define SDVXHOOK_CONFIG_LEDS_DEFAULT_CONTROL_PANEL_AVG_B_PIN_VALUE (-1)
#define SDVXHOOK_CONFIG_LEDS_DEFAULT_WOOFER_AVG_R_PIN_VALUE (-1)
#define SDVXHOOK_CONFIG_LEDS_DEFAULT_WOOFER_AVG_G_PIN_VALUE (-1)
#define SDVXHOOK_CONFIG_LEDS_DEFAULT_WOOFER_AVG_B_PIN_VALUE (-1)
#define SDVXHOOK_CONFIG_LEDS_DEFAULT_V_UNIT_AVG_R_PIN_VALUE (-1)
#define SDVXHOOK_CONFIG_LEDS_DEFAULT_V_UNIT_AVG_G_PIN_VALUE (-1)
#define SDVXHOOK_CONFIG_LEDS_DEFAULT_V_UNIT_AVG_B_PIN_VALUE (-1)
#define SDVXHOOK_CONFIG_LEDS_DEFAULT_IC_CARD_READER_R_PIN_VALUE (-1)
#define SDVXHOOK_CONFIG_LEDS_DEFAULT_IC_CARD_READER_G_PIN_VALUE (-1)
#define SDVXHOOK_CONFIG_LEDS_DEFAULT_IC_CARD_READER_B_PIN_VALUE (-1)

struct light_config_detail {
    int tape_led_light;
    const char *led_name;
    const char *config_key;
    int config_default;
    size_t offset;
};

// Due to a quirk of the IO, wing L/R cannot be set independently.
// You should only assign one of the Left/Right to the up/low LEDs, otherwise
// brightness can be weird. You can assign up to 3 pins per LED, just end the
// list with -1. e.g. {0, 1, 2, -1}. I wouldn't recommend this due to the above
// issue. refer to src/main/sdvxio-bio2/sdvxio.c::sdvx_io_write_output
static const struct light_config_detail light_config_details[] = {
    {TITLE_AVG_R,
     "Title Avg R",
     SDVXHOOK_CONFIG_LEDS_TITLE_AVG_R_PIN_KEY,
     SDVXHOOK_CONFIG_LEDS_DEFAULT_TITLE_AVG_R_PIN_VALUE,
     offsetof(struct sdvxhook_config_leds, title_avg_r_pin)},
    {TITLE_AVG_G,
     "Title Avg G",
     SDVXHOOK_CONFIG_LEDS_TITLE_AVG_G_PIN_KEY,
     SDVXHOOK_CONFIG_LEDS_DEFAULT_TITLE_AVG_G_PIN_VALUE,
     offsetof(struct sdvxhook_config_leds, title_avg_g_pin)},
    {TITLE_AVG_B,
     "Title Avg B",
     SDVXHOOK_CONFIG_LEDS_TITLE_AVG_B_PIN_KEY,
     SDVXHOOK_CONFIG_LEDS_DEFAULT_TITLE_AVG_B_PIN_VALUE,
     offsetof(struct sdvxhook_config_leds, title_avg_b_pin)},
    {UPPER_LEFT_SPEAKER_AVG_R,
     "Upper Left Speaker Avg R",
     SDVXHOOK_CONFIG_LEDS_UPPER_LEFT_SPEAKER_AVG_R_PIN_KEY,
     SDVXHOOK_CONFIG_LEDS_DEFAULT_UPPER_LEFT_SPEAKER_AVG_R_PIN_VALUE,
     offsetof(struct sdvxhook_config_leds, upper_left_speaker_avg_r_pin)},
    {UPPER_LEFT_SPEAKER_AVG_G,
     "Upper Left Speaker Avg G",
     SDVXHOOK_CONFIG_LEDS_UPPER_LEFT_SPEAKER_AVG_G_PIN_KEY,
     SDVXHOOK_CONFIG_LEDS_DEFAULT_UPPER_LEFT_SPEAKER_AVG_G_PIN_VALUE,
     offsetof(struct sdvxhook_config_leds, upper_left_speaker_avg_g_pin)},
    {UPPER_LEFT_SPEAKER_AVG_B,
     "Upper Left Speaker Avg B",
     SDVXHOOK_CONFIG_LEDS_UPPER_LEFT_SPEAKER_AVG_B_PIN_KEY,
     SDVXHOOK_CONFIG_LEDS_DEFAULT_UPPER_LEFT_SPEAKER_AVG_B_PIN_VALUE,
     offsetof(struct sdvxhook_config_leds, upper_left_speaker_avg_b_pin)},
    {UPPER_RIGHT_SPEAKER_AVG_R,
     "Upper Right Speaker Avg R",
     SDVXHOOK_CONFIG_LEDS_UPPER_RIGHT_SPEAKER_AVG_R_PIN_KEY,
     SDVXHOOK_CONFIG_LEDS_DEFAULT_UPPER_RIGHT_SPEAKER_AVG_R_PIN_VALUE,
     offsetof(struct sdvxhook_config_leds, upper_right_speaker_avg_r_pin)},
    {UPPER_RIGHT_SPEAKER_AVG_G,
     "Upper Right Speaker Avg G",
     SDVXHOOK_CONFIG_LEDS_UPPER_RIGHT_SPEAKER_AVG_G_PIN_KEY,
     SDVXHOOK_CONFIG_LEDS_DEFAULT_UPPER_RIGHT_SPEAKER_AVG_G_PIN_VALUE,
     offsetof(struct sdvxhook_config_leds, upper_right_speaker_avg_g_pin)},
    {UPPER_RIGHT_SPEAKER_AVG_B,
     "Upper Right Speaker Avg B",
     SDVXHOOK_CONFIG_LEDS_UPPER_RIGHT_SPEAKER_AVG_B_PIN_KEY,
     SDVXHOOK_CONFIG_LEDS_DEFAULT_UPPER_RIGHT_SPEAKER_AVG_B_PIN_VALUE,
     offsetof(struct sdvxhook_config_leds, upper_right_speaker_avg_b_pin)},
    {LEFT_WING_AVG_R,
     "Left Wing Avg R",
     SDVXHOOK_CONFIG_LEDS_LEFT_WING_AVG_R_PIN_KEY,
     SDVXHOOK_CONFIG_LEDS_DEFAULT_LEFT_WING_AVG_R_PIN_VALUE,
     offsetof(struct sdvxhook_config_leds, left_wing_avg_r_pin)},
    {LEFT_WING_AVG_G,
     "Left Wing Avg G",
     SDVXHOOK_CONFIG_LEDS_LEFT_WING_AVG_G_PIN_KEY,
     SDVXHOOK_CONFIG_LEDS_DEFAULT_LEFT_WING_AVG_G_PIN_VALUE,
     offsetof(struct sdvxhook_config_leds, left_wing_avg_g_pin)},
    {LEFT_WING_AVG_B,
     "Left Wing Avg B",
     SDVXHOOK_CONFIG_LEDS_LEFT_WING_AVG_B_PIN_KEY,
     SDVXHOOK_CONFIG_LEDS_DEFAULT_LEFT_WING_AVG_B_PIN_VALUE,
     offsetof(struct sdvxhook_config_leds, left_wing_avg_b_pin)},
    {RIGHT_WING_AVG_R,
     "Right Wing Avg R",
     SDVXHOOK_CONFIG_LEDS_RIGHT_WING_AVG_R_PIN_KEY,
     SDVXHOOK_CONFIG_LEDS_DEFAULT_RIGHT_WING_AVG_R_PIN_VALUE,
     offsetof(struct sdvxhook_config_leds, right_wing_avg_r_pin)},
    {RIGHT_WING_AVG_G,
     "Right Wing Avg G",
     SDVXHOOK_CONFIG_LEDS_RIGHT_WING_AVG_G_PIN_KEY,
     SDVXHOOK_CONFIG_LEDS_DEFAULT_RIGHT_WING_AVG_G_PIN_VALUE,
     offsetof(struct sdvxhook_config_leds, right_wing_avg_g_pin)},
    {RIGHT_WING_AVG_B,
     "Right Wing Avg B",
     SDVXHOOK_CONFIG_LEDS_RIGHT_WING_AVG_B_PIN_KEY,
     SDVXHOOK_CONFIG_LEDS_DEFAULT_RIGHT_WING_AVG_B_PIN_VALUE,
     offsetof(struct sdvxhook_config_leds, right_wing_avg_b_pin)},
    {LOWER_LEFT_SPEAKER_AVG_R,
     "Lower Left Speaker Avg R",
     SDVXHOOK_CONFIG_LEDS_LOWER_LEFT_SPEAKER_AVG_R_PIN_KEY,
     SDVXHOOK_CONFIG_LEDS_DEFAULT_LOWER_LEFT_SPEAKER_AVG_R_PIN_VALUE,
     offsetof(struct sdvxhook_config_leds, lower_left_speaker_avg_r_pin)},
    {LOWER_LEFT_SPEAKER_AVG_G,
     "Lower Left Speaker Avg G",
     SDVXHOOK_CONFIG_LEDS_LOWER_LEFT_SPEAKER_AVG_G_PIN_KEY,
     SDVXHOOK_CONFIG_LEDS_DEFAULT_LOWER_LEFT_SPEAKER_AVG_G_PIN_VALUE,
     offsetof(struct sdvxhook_config_leds, lower_left_speaker_avg_g_pin)},
    {LOWER_LEFT_SPEAKER_AVG_B,
     "Lower Left Speaker Avg B",
     SDVXHOOK_CONFIG_LEDS_LOWER_LEFT_SPEAKER_AVG_B_PIN_KEY,
     SDVXHOOK_CONFIG_LEDS_DEFAULT_LOWER_LEFT_SPEAKER_AVG_B_PIN_VALUE,
     offsetof(struct sdvxhook_config_leds, lower_left_speaker_avg_b_pin)},
    {LOWER_RIGHT_SPEAKER_AVG_R,
     "Lower Right Speaker Avg R",
     SDVXHOOK_CONFIG_LEDS_LOWER_RIGHT_SPEAKER_AVG_R_PIN_KEY,
     SDVXHOOK_CONFIG_LEDS_DEFAULT_LOWER_RIGHT_SPEAKER_AVG_R_PIN_VALUE,
     offsetof(struct sdvxhook_config_leds, lower_right_speaker_avg_r_pin)},
    {LOWER_RIGHT_SPEAKER_AVG_G,
     "Lower Right Speaker Avg G",
     SDVXHOOK_CONFIG_LEDS_LOWER_RIGHT_SPEAKER_AVG_G_PIN_KEY,
     SDVXHOOK_CONFIG_LEDS_DEFAULT_LOWER_RIGHT_SPEAKER_AVG_G_PIN_VALUE,
     offsetof(struct sdvxhook_config_leds, lower_right_speaker_avg_g_pin)},
    {LOWER_RIGHT_SPEAKER_AVG_B,
     "Lower Right Speaker Avg B",
     SDVXHOOK_CONFIG_LEDS_LOWER_RIGHT_SPEAKER_AVG_B_PIN_KEY,
     SDVXHOOK_CONFIG_LEDS_DEFAULT_LOWER_RIGHT_SPEAKER_AVG_B_PIN_VALUE,
     offsetof(struct sdvxhook_config_leds, lower_right_speaker_avg_b_pin)},
    {CONTROL_PANEL_AVG_R,
     "Control Panel Avg R",
     SDVXHOOK_CONFIG_LEDS_CONTROL_PANEL_AVG_R_PIN_KEY,
     SDVXHOOK_CONFIG_LEDS_DEFAULT_CONTROL_PANEL_AVG_R_PIN_VALUE,
     offsetof(struct sdvxhook_config_leds, control_panel_avg_r_pin)},
    {CONTROL_PANEL_AVG_G,
     "Control Panel Avg G",
     SDVXHOOK_CONFIG_LEDS_CONTROL_PANEL_AVG_G_PIN_KEY,
     SDVXHOOK_CONFIG_LEDS_DEFAULT_CONTROL_PANEL_AVG_G_PIN_VALUE,
     offsetof(struct sdvxhook_config_leds, control_panel_avg_g_pin)},
    {CONTROL_PANEL_AVG_B,
     "Control Panel Avg B",
     SDVXHOOK_CONFIG_LEDS_CONTROL_PANEL_AVG_B_PIN_KEY,
     SDVXHOOK_CONFIG_LEDS_DEFAULT_CONTROL_PANEL_AVG_B_PIN_VALUE,
     offsetof(struct sdvxhook_config_leds, control_panel_avg_b_pin)},
    {WOOFER_AVG_R,
     "Woofer Avg R",
     SDVXHOOK_CONFIG_LEDS_WOOFER_AVG_R_PIN_KEY,
     SDVXHOOK_CONFIG_LEDS_DEFAULT_WOOFER_AVG_R_PIN_VALUE,
     offsetof(struct sdvxhook_config_leds, woofer_avg_r_pin)},
    {WOOFER_AVG_G,
     "Woofer Avg G",
     SDVXHOOK_CONFIG_LEDS_WOOFER_AVG_G_PIN_KEY,
     SDVXHOOK_CONFIG_LEDS_DEFAULT_WOOFER_AVG_G_PIN_VALUE,
     offsetof(struct sdvxhook_config_leds, woofer_avg_g_pin)},
    {WOOFER_AVG_B,
     "Woofer Avg B",
     SDVXHOOK_CONFIG_LEDS_WOOFER_AVG_B_PIN_KEY,
     SDVXHOOK_CONFIG_LEDS_DEFAULT_WOOFER_AVG_B_PIN_VALUE,
     offsetof(struct sdvxhook_config_leds, woofer_avg_b_pin)},
    {V_UNIT_AVG_R,
     "V Unit Avg R",
     SDVXHOOK_CONFIG_LEDS_V_UNIT_AVG_R_PIN_KEY,
     SDVXHOOK_CONFIG_LEDS_DEFAULT_V_UNIT_AVG_R_PIN_VALUE,
     offsetof(struct sdvxhook_config_leds, v_unit_avg_r_pin)},
    {V_UNIT_AVG_G,
     "V Unit Avg G",
     SDVXHOOK_CONFIG_LEDS_V_UNIT_AVG_G_PIN_KEY,
     SDVXHOOK_CONFIG_LEDS_DEFAULT_V_UNIT_AVG_G_PIN_VALUE,
     offsetof(struct sdvxhook_config_leds, v_unit_avg_g_pin)},
    {V_UNIT_AVG_B,
     "V Unit Avg B",
     SDVXHOOK_CONFIG_LEDS_V_UNIT_AVG_B_PIN_KEY,
     SDVXHOOK_CONFIG_LEDS_DEFAULT_V_UNIT_AVG_B_PIN_VALUE,
     offsetof(struct sdvxhook_config_leds, v_unit_avg_b_pin)},
    {IC_CARD_READER_R,
     "IC Card Reader R",
     SDVXHOOK_CONFIG_LEDS_IC_CARD_READER_R_PIN_KEY,
     SDVXHOOK_CONFIG_LEDS_DEFAULT_IC_CARD_READER_R_PIN_VALUE,
     offsetof(struct sdvxhook_config_leds, ic_card_reader_r_pin)},
    {IC_CARD_READER_G,
     "IC Card Reader G",
     SDVXHOOK_CONFIG_LEDS_IC_CARD_READER_G_PIN_KEY,
     SDVXHOOK_CONFIG_LEDS_DEFAULT_IC_CARD_READER_G_PIN_VALUE,
     offsetof(struct sdvxhook_config_leds, ic_card_reader_g_pin)},
    {IC_CARD_READER_B,
     "IC Card Reader B",
     SDVXHOOK_CONFIG_LEDS_IC_CARD_READER_B_PIN_KEY,
     SDVXHOOK_CONFIG_LEDS_DEFAULT_IC_CARD_READER_B_PIN_VALUE,
     offsetof(struct sdvxhook_config_leds, ic_card_reader_b_pin)},
};

void sdvxhook_config_leds_init(struct cconfig *config);

void sdvxhook_config_leds_get(
    struct sdvxhook_config_leds *config_leds, struct cconfig *config);

#endif // SDVXHOOK_CONFIG_LEDS_H
