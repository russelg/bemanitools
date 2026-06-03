#ifndef CCONFIG_UTIL_H
#define CCONFIG_UTIL_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "cconfig/cconfig.h"

#include "util/log.h"

bool cconfig_util_get_int(
    struct cconfig *config,
    const char *key,
    int32_t *ret,
    int32_t default_value);

bool cconfig_util_get_float(
    struct cconfig *config, const char *key, float *ret, float default_value);

bool cconfig_util_get_bool(
    struct cconfig *config, const char *key, bool *ret, bool default_value);

bool cconfig_util_get_str(
    struct cconfig *config,
    const char *key,
    char *buffer,
    size_t len,
    const char *default_value);

bool cconfig_util_get_data(
    struct cconfig *config,
    const char *key,
    uint8_t *buffer,
    size_t len,
    const uint8_t *default_value);

void cconfig_util_set_int(
    struct cconfig *config, const char *key, int32_t value, const char *desc);

void cconfig_util_set_float(
    struct cconfig *config, const char *key, float value, const char *desc);

void cconfig_util_set_bool(
    struct cconfig *config, const char *key, bool value, const char *desc);

void cconfig_util_set_str(
    struct cconfig *config,
    const char *key,
    const char *value,
    const char *desc);

void cconfig_util_set_data(
    struct cconfig *config,
    const char *key,
    const uint8_t *value,
    size_t len,
    const char *desc);

/**
 * Parse a delimiter-separated list of integers from config into ret[0..max_len-1].
 * Unused slots are filled with -1. Returns true if key existed and was parsed successfully.
 * On failure, fills ret from default_value[0..default_len-1] and pads remainder with -1.
 * delimiters is passed directly to strtok (e.g. " \t" splits on space or tab).
 */
bool cconfig_util_get_int_array(
    struct cconfig *config,
    const char *key,
    int32_t *ret,
    size_t max_len,
    const int32_t *default_value,
    size_t default_len,
    const char *delimiters);

/**
 * Format value[0..len-1] as a delimiter-separated string and store in config.
 * delimiter is written between elements (e.g. " " produces "1 2 3").
 */
void cconfig_util_set_int_array(
    struct cconfig *config,
    const char *key,
    const int32_t *value,
    size_t len,
    const char *delimiter,
    const char *desc);

void cconfig_util_log(struct cconfig *config, log_formatter_t log_formatter);

#endif