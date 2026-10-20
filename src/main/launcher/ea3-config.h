#ifndef LAUNCHER_EA3_CONFIG_H
#define LAUNCHER_EA3_CONFIG_H

#include "imports/avs.h"

#include "launcher/module.h"

/* N.B. even though this might look like a Konami ABI, this is purely an
   internal data structure. */

struct ea3_ident {
    /* psmapped structure offset can't be zero for some stupid reason */

    uint32_t dummy;

    /* Initialized from ea3-config.xml, then fed back from sidcode_short */

    char model[4];
    char dest[4];
    char spec[4];
    char rev[4];
    char ext[11];

    /* Initialized from ea3-config.xml (hardware_id defaults to MAC addr) */

    char softid[24];
    char hardid[24];
    char pcbid[24];
};

void ea3_ident_init(struct ea3_ident *ident);
bool ea3_ident_from_property(
    struct ea3_ident *ident, struct property *ea3_config);
/**
 * Reads the release code from the given bootstrap config and stores it in the
 * "ext" field of the given ident.
 *
 * The release code of the installed game is what the arcade boot process puts
 * into the "ext" field of the sidcode. It therefore takes precedence over the
 * release code that is configured in the ea3 config, which might be outdated
 * or belong to a different game version.
 *
 * Returns true if a release code was read from the bootstrap config.
 */
bool ea3_ident_ext_from_bootstrap(
    struct ea3_ident *ident, struct property *bootstrap_config);
void ea3_ident_hardid_from_ethernet(struct ea3_ident *ident);
bool ea3_ident_invoke_module_init(
    struct ea3_ident *ident,
    const struct module_context *module,
    struct property_node *app_config);
void ea3_ident_to_property(
    const struct ea3_ident *ident, struct property *ea3_config);
void ea3_ident_replace_property_bool(
    struct property_node *node, const char *name, uint8_t val);
void ea3_ident_replace_property_str(
    struct property_node *node, const char *name, const char *val);

#endif
