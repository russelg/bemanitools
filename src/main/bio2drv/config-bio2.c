#include "cconfig/cconfig-util.h"

#include "bio2drv/config-bio2.h"

#include "util/log.h"

#define BIO2DRV_CONFIG_BIO2_AUTO_KEY "bio2.autodetect"
#define BIO2DRV_CONFIG_BIO2_PORT_KEY "bio2.port"
#define BIO2DRV_CONFIG_BIO2_BAUD_KEY "bio2.baud"
#define BIO2DRV_CONFIG_BIO2_INITIALIZE_AMPS_KEY "bio2.initialize_amps"
#define BIO2DRV_CONFIG_BIO2_AMP_VOLUME_KEY "bio2.amp_volume"

#define BIO2DRV_CONFIG_BIO2_DEFAULT_AUTO_VALUE true
#define BIO2DRV_CONFIG_BIO2_DEFAULT_PORT_VALUE "COM4"
#define BIO2DRV_CONFIG_BIO2_DEFAULT_BAUD_VALUE 115200
#define BIO2DRV_CONFIG_BIO2_DEFAULT_INITIALIZE_AMPS_VALUE true
#define BIO2DRV_CONFIG_BIO2_DEFAULT_AMP_VOLUME_VALUE 48

void bio2drv_config_bio2_init(struct cconfig *config)
{
    cconfig_util_set_bool(
        config,
        BIO2DRV_CONFIG_BIO2_AUTO_KEY,
        BIO2DRV_CONFIG_BIO2_DEFAULT_AUTO_VALUE,
        "Autodetect BIO2 port (default: on)");

    cconfig_util_set_str(
        config,
        BIO2DRV_CONFIG_BIO2_PORT_KEY,
        BIO2DRV_CONFIG_BIO2_DEFAULT_PORT_VALUE,
        "BIO2 serial port. Optional, if autodetect property is turned on");

    cconfig_util_set_int(
        config,
        BIO2DRV_CONFIG_BIO2_BAUD_KEY,
        BIO2DRV_CONFIG_BIO2_DEFAULT_BAUD_VALUE,
        "BIO2 bus baudrate (real devices expect 115200)");

    cconfig_util_set_bool(
        config,
        BIO2DRV_CONFIG_BIO2_INITIALIZE_AMPS_KEY,
        BIO2DRV_CONFIG_BIO2_DEFAULT_INITIALIZE_AMPS_VALUE,
        "Initialize amps on BIO2 init. Needed when running in Valkyrie mode "
        "with BI2X as it does not init amps anymore.");

    cconfig_util_set_int(
        config,
        BIO2DRV_CONFIG_BIO2_AMP_VOLUME_KEY,
        BIO2DRV_CONFIG_BIO2_DEFAULT_AMP_VOLUME_VALUE,
        "Digital amp volume (0-96) 0 is high, 96 is low.");
}

void bio2drv_config_bio2_get(
    struct bio2drv_config_bio2 *config_bio2, struct cconfig *config)
{
    if (!cconfig_util_get_bool(
            config,
            BIO2DRV_CONFIG_BIO2_AUTO_KEY,
            &config_bio2->autodetect,
            BIO2DRV_CONFIG_BIO2_DEFAULT_AUTO_VALUE)) {
        log_warning(
            "Invalid value for key '%s' specified, fallback "
            "to default '%d'",
            BIO2DRV_CONFIG_BIO2_AUTO_KEY,
            BIO2DRV_CONFIG_BIO2_DEFAULT_AUTO_VALUE);
    }

    if (!cconfig_util_get_str(
            config,
            BIO2DRV_CONFIG_BIO2_PORT_KEY,
            config_bio2->port,
            sizeof(config_bio2->port) - 1,
            BIO2DRV_CONFIG_BIO2_DEFAULT_PORT_VALUE)) {
        log_warning(
            "Invalid value for key '%s' specified, fallback "
            "to default '%s'",
            BIO2DRV_CONFIG_BIO2_PORT_KEY,
            BIO2DRV_CONFIG_BIO2_DEFAULT_PORT_VALUE);
    }

    if (!cconfig_util_get_int(
            config,
            BIO2DRV_CONFIG_BIO2_BAUD_KEY,
            &config_bio2->baud,
            BIO2DRV_CONFIG_BIO2_DEFAULT_BAUD_VALUE)) {
        log_warning(
            "Invalid value for key '%s' specified, fallback "
            "to default '%d'",
            BIO2DRV_CONFIG_BIO2_BAUD_KEY,
            BIO2DRV_CONFIG_BIO2_DEFAULT_BAUD_VALUE);
    }

    if (!cconfig_util_get_bool(
            config,
            BIO2DRV_CONFIG_BIO2_INITIALIZE_AMPS_KEY,
            &config_bio2->initialize_amps,
            BIO2DRV_CONFIG_BIO2_DEFAULT_INITIALIZE_AMPS_VALUE)) {
        log_warning(
            "Invalid value for key '%s' specified, fallback "
            "to default '%d'",
            BIO2DRV_CONFIG_BIO2_INITIALIZE_AMPS_KEY,
            BIO2DRV_CONFIG_BIO2_DEFAULT_INITIALIZE_AMPS_VALUE);
    }

    if (!cconfig_util_get_int(
            config,
            BIO2DRV_CONFIG_BIO2_AMP_VOLUME_KEY,
            &config_bio2->amp_volume,
            BIO2DRV_CONFIG_BIO2_DEFAULT_AMP_VOLUME_VALUE)) {
        log_warning(
            "Invalid value for key '%s' specified, fallback "
            "to default '%d'",
            BIO2DRV_CONFIG_BIO2_AMP_VOLUME_KEY,
            BIO2DRV_CONFIG_BIO2_DEFAULT_AMP_VOLUME_VALUE);
    }
}
