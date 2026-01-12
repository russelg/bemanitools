#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "util/log.h"

#include "spice-sdvxio/cJSON.h"
#include "spice-sdvxio/spice_wrappers.h"

uint64_t spice_msg_gen_id()
{
    static uint64_t id_global = 0;

    if (id_global == 0) {
        srand((unsigned int) time(NULL));
        id_global = ((uint64_t) rand() << 32) | rand();
    } else {
        id_global++;
    }

    return id_global;
}

static char *spice_doc2str(cJSON *doc)
{
    char *str = cJSON_Print(doc);
    return str;
}

static cJSON *spice_request_gen(const char *module, const char *function)
{
    cJSON *doc = cJSON_CreateObject();
    cJSON_AddNumberToObject(doc, "id", (double) spice_msg_gen_id());
    cJSON_AddStringToObject(doc, "module", module);
    cJSON_AddStringToObject(doc, "function", function);
    cJSON_AddArrayToObject(doc, "params");
    return doc;
}

static cJSON *spice_response_get(char *json)
{
    if (!json) {
        return NULL;
    }

    cJSON *doc = cJSON_Parse(json);
    free(json);

    if (!doc) {
        return NULL;
    }

    cJSON *errors = cJSON_GetObjectItemCaseSensitive(doc, "errors");
    if (!cJSON_IsArray(errors) || cJSON_GetArraySize(errors) > 0) {
        cJSON_Delete(doc);
        return NULL;
    }

    cJSON *data = cJSON_GetObjectItemCaseSensitive(doc, "data");
    if (!cJSON_IsArray(data)) {
        cJSON_Delete(doc);
        return NULL;
    }

    return doc;
}

bool spice_analogs_write(struct spice_connection *con, struct spice_analog_state *states, size_t count)
{
    cJSON *req = spice_request_gen("analogs", "write");
    cJSON *params = cJSON_GetObjectItemCaseSensitive(req, "params");

    for (size_t i = 0; i < count; i++) {
        cJSON *state = cJSON_CreateArray();
        cJSON_AddItemToArray(params, state);

        cJSON *name = cJSON_CreateString(states[i].name);
        cJSON_AddItemToArray(state, name);
        cJSON *value = cJSON_CreateNumber(states[i].value);
        cJSON_AddItemToArray(state, value);
    }

    char *req_str = spice_doc2str(req);
    cJSON_Delete(req);

    cJSON *res = spice_response_get(spice_connection_request(con, req_str));
    free(req_str);

    if (!res) {
        return false;
    }

    cJSON_Delete(res);
    return true;
}

bool spice_buttons_write(struct spice_connection *con, struct spice_button_state *states, size_t count)
{
    cJSON *req = spice_request_gen("buttons", "write");
    cJSON *params = cJSON_GetObjectItemCaseSensitive(req, "params");

    for (size_t i = 0; i < count; i++) {
        cJSON *state = cJSON_CreateArray();
        cJSON_AddItemToArray(params, state);

        cJSON *name = cJSON_CreateString(states[i].name);
        cJSON_AddItemToArray(state, name);
        cJSON *value = cJSON_CreateNumber(states[i].value);
        cJSON_AddItemToArray(state, value);
    }

    char *req_str = spice_doc2str(req);
    cJSON_Delete(req);

    cJSON *res = spice_response_get(spice_connection_request(con, req_str));
    free(req_str);

    if (!res) {
        return false;
    }

    cJSON_Delete(res);
    return true;
}

bool spice_coin_set(struct spice_connection *con, int coins)
{
    cJSON *req = spice_request_gen("coin", "set");
    cJSON *params = cJSON_GetObjectItemCaseSensitive(req, "params");

    cJSON *amount = cJSON_CreateNumber(coins);
    cJSON_AddItemToArray(params, amount);

    char *req_str = spice_doc2str(req);
    cJSON_Delete(req);

    cJSON *res = spice_response_get(spice_connection_request(con, req_str));
    free(req_str);

    if (!res) {
        return false;
    }

    cJSON_Delete(res);
    return true;
}

bool spice_coin_blocker_get(struct spice_connection *con, bool *closed)
{
    cJSON *req = spice_request_gen("coin", "blocker_get");
    char *req_str = spice_doc2str(req);
    cJSON_Delete(req);

    cJSON *res = spice_response_get(spice_connection_request(con, req_str));
    free(req_str);

    if (!res) {
        return false;
    }

    cJSON *array = cJSON_GetObjectItemCaseSensitive(res, "data");
    if (!cJSON_IsArray(array)) {
        cJSON_Delete(res);
        return false;
    }

    cJSON *item = cJSON_GetArrayItem(array, 0);
    if (!item) {
        cJSON_Delete(res);
        return false;
    }

    *closed = cJSON_IsTrue(item);

    cJSON_Delete(res);
    return true;
}

bool spice_lights_read(struct spice_connection *con, struct spice_light_state **states, size_t *count)
{
    cJSON *req = spice_request_gen("lights", "read");
    char *req_str = spice_doc2str(req);
    cJSON_Delete(req);

    cJSON *res = spice_response_get(spice_connection_request(con, req_str));
    free(req_str);

    if (!res) {
        return false;
    }

    cJSON *array = cJSON_GetObjectItemCaseSensitive(res, "data");
    if (!cJSON_IsArray(array)) {
        cJSON_Delete(res);
        return false;
    }

    *count = (size_t) cJSON_GetArraySize(array);
    *states = calloc(*count, sizeof(struct spice_light_state));

    for (size_t i = 0; i < *count; i++) {
        cJSON *item = cJSON_GetArrayItem(array, (int) i);
        (*states)[i].name = strdup(cJSON_GetArrayItem(item, 0)->valuestring);
        (*states)[i].value = (float) cJSON_GetArrayItem(item, 1)->valuedouble;
        (*states)[i].enabled = (bool) cJSON_GetArrayItem(item, 2)->valueint;
    }

    cJSON_Delete(res);
    return true;
}

void spice_lights_free(struct spice_light_state *states, size_t count)
{
    for (size_t i = 0; i < count; i++) {
        free(states[i].name);
    }
    free(states);
}

bool spice_control_session_refresh(struct spice_connection *con)
{
    cJSON *req = spice_request_gen("control", "session_refresh");
    char *req_str = spice_doc2str(req);
    cJSON_Delete(req);

    cJSON *res = spice_response_get(spice_connection_request(con, req_str));
    free(req_str);

    if (!res) {
        return false;
    }

    cJSON_Delete(res);
    return true;
}

bool spice_info_launcher(struct spice_connection *con, struct spice_info_launcher *info)
{
    cJSON *req = spice_request_gen("info", "launcher");
    char *req_str = spice_doc2str(req);
    cJSON_Delete(req);

    cJSON *res = spice_response_get(spice_connection_request(con, req_str));
    free(req_str);

    if (!res) {
        return false;
    }

    cJSON *data = cJSON_GetObjectItemCaseSensitive(res, "data");
    cJSON *obj = cJSON_GetArrayItem(data, 0);

    info->version = strdup(cJSON_GetObjectItemCaseSensitive(obj, "version")->valuestring);
    info->compile_date = strdup(cJSON_GetObjectItemCaseSensitive(obj, "compile_date")->valuestring);
    info->compile_time = strdup(cJSON_GetObjectItemCaseSensitive(obj, "compile_time")->valuestring);
    info->system_time = strdup(cJSON_GetObjectItemCaseSensitive(obj, "system_time")->valuestring);

    cJSON_Delete(res);
    return true;
}

void spice_info_launcher_free(struct spice_info_launcher *info)
{
    free(info->version);
    free(info->compile_date);
    free(info->compile_time);
    free(info->system_time);
}
