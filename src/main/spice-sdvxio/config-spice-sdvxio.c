#include "cconfig/cconfig-main.h"
#include "cconfig/cconfig-util.h"

#include "spice-sdvxio/config-spice-sdvxio.h"

#include "util/log.h"

#define SPICE_SDVXIO_CONFIG_ENABLE_KEYLIGHT_KEY "sdvxio.enable_keylight"
#define SPICE_SDVXIO_CONFIG_AMP_VOLUME_KEY "sdvxio.amp_volume"
#define SPICE_SDVXIO_CONFIG_HOST_KEY "spice.host"
#define SPICE_SDVXIO_CONFIG_PORT_KEY "spice.port"
#define SPICE_SDVXIO_CONFIG_PASSWORD_KEY "spice.password"
#define SPICE_SDVXIO_CONFIG_DEFAULT_ENABLE_KEYLIGHT_VALUE true
#define SPICE_SDVXIO_CONFIG_DEFAULT_RELATIVE_ANALOG_VALUE false
#define SPICE_SDVXIO_CONFIG_DEFAULT_PWM_WINGS_VALUE 128
#define SPICE_SDVXIO_CONFIG_DEFAULT_PWM_CONTROLLER_VALUE 64
#define SPICE_SDVXIO_CONFIG_DEFAULT_AMP_VOLUME_VALUE 48
#define SPICE_SDVXIO_CONFIG_DEFAULT_HOST_VALUE "localhost"
#define SPICE_SDVXIO_CONFIG_DEFAULT_PORT_VALUE 1337
#define SPICE_SDVXIO_CONFIG_DEFAULT_PASSWORD_VALUE "password"

static void spice_sdvxio_config_init(struct cconfig *config)
{
    cconfig_util_set_bool(
        config,
        SPICE_SDVXIO_CONFIG_ENABLE_KEYLIGHT_KEY,
        SPICE_SDVXIO_CONFIG_DEFAULT_ENABLE_KEYLIGHT_VALUE,
        "Enable input based key lighting");

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
}

bool get_spice_sdvxio_config(struct spice_sdvxio_config *config_out)
{
    struct cconfig *config;

    config = cconfig_init();

    spice_sdvxio_config_init(config);

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

    cconfig_finit(config);

    return true;
}
