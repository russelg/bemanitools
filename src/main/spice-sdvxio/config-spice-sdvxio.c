#include "cconfig/cconfig-main.h"
#include "cconfig/cconfig-util.h"

#include "sdvxhook-util/config-leds.h"
#include "spice-sdvxio/config-spice-sdvxio.h"

#include "util/log.h"

#define SPICE_SDVXIO_CONFIG_ENABLE_KEYLIGHT_KEY "sdvxio.enable_keylight"
#define SPICE_SDVXIO_CONFIG_AMP_VOLUME_KEY "sdvxio.amp_volume"
#define SPICE_SDVXIO_CONFIG_SERVICE_COIN_KEY "sdvxio.service_coin"
#define SPICE_SDVXIO_CONFIG_HOST_KEY "spice.host"
#define SPICE_SDVXIO_CONFIG_PORT_KEY "spice.port"
#define SPICE_SDVXIO_CONFIG_PASSWORD_KEY "spice.password"
#define SPICE_SDVXIO_CONFIG_ANALOG_ENABLE_SLEEP_KEY "sdvxio.analog_enable_sleep"
#define SPICE_SDVXIO_CONFIG_ANALOG_SNAP_MULTIPLIER_KEY \
    "sdvxio.analog_snap_multiplier"
#define SPICE_SDVXIO_CONFIG_DEFAULT_ENABLE_KEYLIGHT_VALUE true
#define SPICE_SDVXIO_CONFIG_DEFAULT_AMP_VOLUME_VALUE 48
#define SPICE_SDVXIO_CONFIG_DEFAULT_SERVICE_COIN_VALUE false
#define SPICE_SDVXIO_CONFIG_DEFAULT_HOST_VALUE "localhost"
#define SPICE_SDVXIO_CONFIG_DEFAULT_PORT_VALUE 1337
#define SPICE_SDVXIO_CONFIG_DEFAULT_PASSWORD_VALUE "password"
#define SPICE_SDVXIO_CONFIG_DEFAULT_ANALOG_ENABLE_SLEEP_VALUE true
#define SPICE_SDVXIO_CONFIG_DEFAULT_ANALOG_SNAP_MULTIPLIER_VALUE 0.1f

static void spice_sdvxio_config_init(struct cconfig *config)
{
    cconfig_util_set_bool(
        config,
        SPICE_SDVXIO_CONFIG_ENABLE_KEYLIGHT_KEY,
        SPICE_SDVXIO_CONFIG_DEFAULT_ENABLE_KEYLIGHT_VALUE,
        "Enable input based key lighting");

    cconfig_util_set_bool(
        config,
        SPICE_SDVXIO_CONFIG_SERVICE_COIN_KEY,
        SPICE_SDVXIO_CONFIG_DEFAULT_SERVICE_COIN_VALUE,
        "Send the service button instead of coin insert");

    cconfig_util_set_int(
        config,
        SPICE_SDVXIO_CONFIG_AMP_VOLUME_KEY,
        SPICE_SDVXIO_CONFIG_DEFAULT_AMP_VOLUME_VALUE,
        "SDVXIO digital amp volume (0-96) 0 is high, 96 is low.");

    cconfig_util_set_str(
        config,
        SPICE_SDVXIO_CONFIG_HOST_KEY,
        SPICE_SDVXIO_CONFIG_DEFAULT_HOST_VALUE,
        "Host of spice server");

    cconfig_util_set_int(
        config,
        SPICE_SDVXIO_CONFIG_PORT_KEY,
        SPICE_SDVXIO_CONFIG_DEFAULT_PORT_VALUE,
        "Port of spice server");

    cconfig_util_set_str(
        config,
        SPICE_SDVXIO_CONFIG_PASSWORD_KEY,
        SPICE_SDVXIO_CONFIG_DEFAULT_PASSWORD_VALUE,
        "Password of spice server");

    cconfig_util_set_bool(
        config,
        SPICE_SDVXIO_CONFIG_ANALOG_ENABLE_SLEEP_KEY,
        SPICE_SDVXIO_CONFIG_DEFAULT_ANALOG_ENABLE_SLEEP_VALUE,
        "Enabling sleep will cause values to take less time to stop changing "
        "and potentially stop changing more abruptly, where as disabling sleep "
        "will cause values to ease into their correct position smoothly");

    cconfig_util_set_float(
        config,
        SPICE_SDVXIO_CONFIG_ANALOG_SNAP_MULTIPLIER_KEY,
        SPICE_SDVXIO_CONFIG_DEFAULT_ANALOG_SNAP_MULTIPLIER_VALUE,
        "Controls the amount of easing (0.0-1.0). Increase this to lessen the "
        "amount of easing (such as 0.1) and make the responsive values more "
        "responsive, but doing so may cause more noise to seep through if "
        "sleep is not enabled");
}

static void spice_sdvxio_config_get(
    struct spice_sdvxio_config *spice_config, struct cconfig *config)
{
    if (!cconfig_util_get_bool(
            config,
            SPICE_SDVXIO_CONFIG_ENABLE_KEYLIGHT_KEY,
            &spice_config->enable_keylight,
            SPICE_SDVXIO_CONFIG_DEFAULT_ENABLE_KEYLIGHT_VALUE)) {
        log_warning(
            "Invalid value for key '%s' specified, fallback "
            "to default '%d'",
            SPICE_SDVXIO_CONFIG_ENABLE_KEYLIGHT_KEY,
            SPICE_SDVXIO_CONFIG_DEFAULT_ENABLE_KEYLIGHT_VALUE);
    }

    if (!cconfig_util_get_bool(
            config,
            SPICE_SDVXIO_CONFIG_SERVICE_COIN_KEY,
            &spice_config->service_coin,
            SPICE_SDVXIO_CONFIG_DEFAULT_SERVICE_COIN_VALUE)) {
        log_warning(
            "Invalid value for key '%s' specified, fallback "
            "to default '%d'",
            SPICE_SDVXIO_CONFIG_SERVICE_COIN_KEY,
            SPICE_SDVXIO_CONFIG_DEFAULT_SERVICE_COIN_VALUE);
    }

    if (!cconfig_util_get_int(
            config,
            SPICE_SDVXIO_CONFIG_AMP_VOLUME_KEY,
            &spice_config->amp_volume,
            SPICE_SDVXIO_CONFIG_DEFAULT_AMP_VOLUME_VALUE)) {
        log_warning(
            "Invalid value for key '%s' specified, fallback "
            "to default '%d'",
            SPICE_SDVXIO_CONFIG_AMP_VOLUME_KEY,
            SPICE_SDVXIO_CONFIG_DEFAULT_AMP_VOLUME_VALUE);
    }

    if (!cconfig_util_get_str(
            config,
            SPICE_SDVXIO_CONFIG_HOST_KEY,
            spice_config->host,
            sizeof(spice_config->host),
            SPICE_SDVXIO_CONFIG_DEFAULT_HOST_VALUE)) {
        log_warning(
            "Invalid value for key '%s' specified, fallback "
            "to default '%s'",
            SPICE_SDVXIO_CONFIG_HOST_KEY,
            SPICE_SDVXIO_CONFIG_DEFAULT_HOST_VALUE);
    }

    if (!cconfig_util_get_int(
            config,
            SPICE_SDVXIO_CONFIG_PORT_KEY,
            &spice_config->port,
            SPICE_SDVXIO_CONFIG_DEFAULT_PORT_VALUE)) {
        log_warning(
            "Invalid value for key '%s' specified, fallback "
            "to default '%d'",
            SPICE_SDVXIO_CONFIG_PORT_KEY,
            SPICE_SDVXIO_CONFIG_DEFAULT_PORT_VALUE);
    }

    if (!cconfig_util_get_str(
            config,
            SPICE_SDVXIO_CONFIG_PASSWORD_KEY,
            spice_config->password,
            sizeof(spice_config->password),
            SPICE_SDVXIO_CONFIG_DEFAULT_PASSWORD_VALUE)) {
        log_warning(
            "Invalid value for key '%s' specified, fallback "
            "to default '%s'",
            SPICE_SDVXIO_CONFIG_PASSWORD_KEY,
            SPICE_SDVXIO_CONFIG_DEFAULT_PASSWORD_VALUE);
    }

    if (!cconfig_util_get_bool(
            config,
            SPICE_SDVXIO_CONFIG_ANALOG_ENABLE_SLEEP_KEY,
            &spice_config->analog_enable_sleep,
            SPICE_SDVXIO_CONFIG_DEFAULT_ANALOG_ENABLE_SLEEP_VALUE)) {
        log_warning(
            "Invalid value for key '%s' specified, fallback "
            "to default '%d'",
            SPICE_SDVXIO_CONFIG_ANALOG_ENABLE_SLEEP_KEY,
            SPICE_SDVXIO_CONFIG_DEFAULT_ANALOG_ENABLE_SLEEP_VALUE);
    }

    if (!cconfig_util_get_float(
            config,
            SPICE_SDVXIO_CONFIG_ANALOG_SNAP_MULTIPLIER_KEY,
            &spice_config->analog_snap_multiplier,
            SPICE_SDVXIO_CONFIG_DEFAULT_ANALOG_SNAP_MULTIPLIER_VALUE)) {
        log_warning(
            "Invalid value for key '%s' specified, fallback "
            "to default '%f'",
            SPICE_SDVXIO_CONFIG_ANALOG_SNAP_MULTIPLIER_KEY,
            SPICE_SDVXIO_CONFIG_DEFAULT_ANALOG_SNAP_MULTIPLIER_VALUE);
    }
}

bool get_spice_sdvxio_config(
    struct spice_sdvxio_config *config_out,
    struct sdvxhook_config_leds *leds_out)
{
    struct cconfig *config;

    config = cconfig_init();

    spice_sdvxio_config_init(config);
    sdvxhook_config_leds_init(config);

    if (!cconfig_main_config_init(
            config,
            "--config",
            "spice-sdvxio.conf",
            "--help",
            "-h",
            "spice-sdvxio",
            CCONFIG_CMD_USAGE_OUT_STDOUT)) {
        cconfig_finit(config);
        return false;
    }

    spice_sdvxio_config_get(config_out, config);
    sdvxhook_config_leds_get(leds_out, config);

    cconfig_finit(config);

    return true;
}
