#ifndef SPICE_CONNECTION_H
#define SPICE_CONNECTION_H

#include <stdbool.h>
#include <stdint.h>
#include <winsock2.h>

#include "spice-sdvxio/spice_rc4.h"

struct spice_connection {
    char *host;
    uint16_t port;
    char *password;
    SOCKET socket;
    struct spice_rc4 *cipher;
};

struct spice_connection *spice_connection_open(const char *host, uint16_t port, const char *password);
void spice_connection_close(struct spice_connection *con);
bool spice_connection_check(struct spice_connection *con);
void spice_connection_change_pass(struct spice_connection *con, const char *password);
char *spice_connection_request(struct spice_connection *con, const char *json);

#endif
