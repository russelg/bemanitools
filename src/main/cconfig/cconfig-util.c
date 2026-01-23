#define LOG_MODULE "cconfig-util"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cconfig/cconfig-util.h"

#include "util/hex.h"
#include "util/log.h"
#include "util/mem.h"
#include "util/str.h"

bool cconfig_util_get_int(
    struct cconfig *config,
    const char *key,
    int32_t *ret,
    int32_t default_value)
{
    struct cconfig_entry *entry;

    log_assert(config);
    log_assert(key);

    entry = cconfig_get(config, key);

    if (entry) {
        if (sscanf(entry->value, "%d", ret) == 1) {
            return true;
        }
    }

    *ret = default_value;
    return false;
}

bool cconfig_util_get_int_array(
    struct cconfig *config,
    const char *key,
    int32_t *ret,
    size_t len,
    const int32_t *default_value,
    const char *delimiter)
{
    struct cconfig_entry *entry;
    const char *delim;
    char *value_dup;
    char *ctx;
    char *tok;
    size_t idx;

    log_assert(config);
    log_assert(key);
    log_assert(ret);
    log_assert(default_value);
    log_assert(len);

    entry = cconfig_get(config, key);
    if (entry && entry->value && entry->value[0]) {
        delim = delimiter ? delimiter : " \t";
        value_dup = str_dup(entry->value);

        ctx = NULL;
        tok = strtok_r(value_dup, delim, &ctx);
        idx = 0;

        while (tok && idx < len) {
            char *endptr;
            long value;

            value = strtol(tok, &endptr, 10);
            if (endptr == tok || *endptr != '\0' || value < INT32_MIN ||
                value > INT32_MAX) {
                free(value_dup);
                memcpy(ret, default_value, len * sizeof(*ret));
                return false;
            }

            ret[idx++] = (int32_t) value;
            tok = strtok_r(NULL, delim, &ctx);
        }

        for (; idx < len; idx++) {
            ret[idx] = default_value[idx];
        }

        free(value_dup);
        return true;
    }

    memcpy(ret, default_value, len * sizeof(*ret));
    return false;
}

bool cconfig_util_get_float(
    struct cconfig *config, const char *key, float *ret, float default_value)
{
    struct cconfig_entry *entry;

    log_assert(config);
    log_assert(key);

    entry = cconfig_get(config, key);

    if (entry) {
        if (sscanf(entry->value, "%f", ret) == 1) {
            return true;
        }
    }

    *ret = default_value;
    return false;
}

bool cconfig_util_get_bool(
    struct cconfig *config, const char *key, bool *ret, bool default_value)
{
    struct cconfig_entry *entry;

    log_assert(config);
    log_assert(key);

    entry = cconfig_get(config, key);

    if (entry) {
        if (!strcmp(entry->value, "true")) {
            *ret = true;
            return true;
        } else if (!strcmp(entry->value, "false")) {
            *ret = false;
            return true;
        }
    }

    *ret = default_value;
    return false;
}

bool cconfig_util_get_str(
    struct cconfig *config,
    const char *key,
    char *buffer,
    size_t len,
    const char *default_value)
{
    struct cconfig_entry *entry;
    size_t str_len;

    log_assert(config);
    log_assert(key);

    entry = cconfig_get(config, key);

    if (entry) {
        str_len = strlen(entry->value);

        if (str_len <= len) {
            strcpy(buffer, entry->value);
            return true;
        }
    }

    strcpy(buffer, default_value);
    return false;
}

bool cconfig_util_get_data(
    struct cconfig *config,
    const char *key,
    uint8_t *buffer,
    size_t len,
    const uint8_t *default_value)
{
    size_t res_len;
    struct cconfig_entry *entry;

    log_assert(config);
    log_assert(key);
    log_assert(len);

    entry = cconfig_get(config, key);

    if (entry) {
        res_len = strlen(entry->value);
        res_len = res_len / 2 + res_len % 2;

        if (len <= res_len) {
            if (hex_decode(buffer, len, entry->value, strlen(entry->value))) {
                return true;
            }
        }
    }

    memcpy(buffer, default_value, len);
    return false;
}

void cconfig_util_set_int(
    struct cconfig *config, const char *key, int32_t value, const char *desc)
{
    char *str;
    size_t str_len;

    log_assert(config);
    log_assert(key);
    log_assert(desc);

    str_len = snprintf(NULL, 0, "%d", value) + 1;
    str = xmalloc(str_len);
    snprintf(str, str_len, "%d", value);

    cconfig_set(config, key, str, desc);

    free(str);
}

void cconfig_util_set_int_array(
    struct cconfig *config,
    const char *key,
    const int32_t *value,
    size_t count,
    const char *delimiter,
    const char *desc)
{
    const char *delim;
    size_t delim_len;
    size_t str_len;
    char *str;
    char *pos;
    size_t remaining;

    log_assert(config);
    log_assert(key);
    log_assert(desc);
    log_assert(value);

    delim = delimiter ? delimiter : " ";
    delim_len = strlen(delim);
    str_len = 1;

    for (size_t i = 0; i < count; i++) {
        str_len += snprintf(NULL, 0, "%d", value[i]);
        if (i + 1 < count) {
            str_len += delim_len;
        }
    }

    str = xmalloc(str_len);
    pos = str;
    remaining = str_len;

    for (size_t i = 0; i < count; i++) {
        int written = (int) str_format(pos, remaining, "%d", value[i]);
        pos += written;
        remaining -= (size_t) written;

        if (i + 1 < count && delim_len > 0) {
            memcpy(pos, delim, delim_len);
            pos += delim_len;
            remaining -= delim_len;
        }
    }

    *pos = '\0';
    cconfig_set(config, key, str, desc);
    free(str);
}

void cconfig_util_set_float(
    struct cconfig *config, const char *key, float value, const char *desc)
{
    char *str;
    size_t str_len;

    log_assert(config);
    log_assert(key);
    log_assert(desc);

    str_len = snprintf(NULL, 0, "%f", value) + 1;
    str = xmalloc(str_len);
    snprintf(str, str_len, "%f", value);

    cconfig_set(config, key, str, desc);

    free(str);
}

void cconfig_util_set_bool(
    struct cconfig *config, const char *key, bool value, const char *desc)
{
    log_assert(config);
    log_assert(key);
    log_assert(desc);

    cconfig_set(config, key, value ? "true" : "false", desc);
}

void cconfig_util_set_str(
    struct cconfig *config,
    const char *key,
    const char *value,
    const char *desc)
{
    log_assert(config);
    log_assert(key);
    log_assert(desc);
    log_assert(value);

    cconfig_set(config, key, value, desc);
}

void cconfig_util_set_data(
    struct cconfig *config,
    const char *key,
    const uint8_t *value,
    size_t len,
    const char *desc)
{
    char *str;
    size_t str_len;

    log_assert(config);
    log_assert(key);
    log_assert(desc);

    str_len = len * 2 + 1;
    str = xmalloc(str_len);
    hex_encode_uc(value, len, str, str_len);

    cconfig_set(config, key, str, desc);

    free(str);
}

void cconfig_util_log(struct cconfig *config, log_formatter_t log_formatter)
{
    for (uint32_t i = 0; i < config->nentries; i++) {
        log_formatter(
            LOG_MODULE,
            "%s=%s",
            config->entries[i].key,
            config->entries[i].value);
    }
}
