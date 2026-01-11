#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ws2tcpip.h>

#include "spice-sdvxio/spice_connection.h"

#define RECEIVE_BUFFER_SIZE (64 * 1024)
#define RECEIVE_TIMEOUT 1000

static void spice_connection_cipher_alloc(struct spice_connection *con)
{
    if (con->cipher) {
        free(con->cipher);
        con->cipher = NULL;
    }

    if (con->password && strlen(con->password) > 0) {
        con->cipher = malloc(sizeof(struct spice_rc4));
        spice_rc4_init(con->cipher, (const uint8_t *) con->password, strlen(con->password));
    }
}

struct spice_connection *spice_connection_open(const char *host, uint16_t port, const char *password)
{
    struct spice_connection *con = calloc(1, sizeof(struct spice_connection));
    con->host = strdup(host);
    con->port = port;
    if (password) {
        con->password = strdup(password);
    }
    con->socket = INVALID_SOCKET;

    WSADATA wsa_data;
    int error = WSAStartup(MAKEWORD(2, 2), &wsa_data);
    if (error) {
        fprintf(stderr, "Failed to start WSA: %d\n", error);
        exit(1);
    }

    return con;
}

void spice_connection_close(struct spice_connection *con)
{
    if (!con) {
        return;
    }

    if (con->socket != INVALID_SOCKET) {
        closesocket(con->socket);
    }

    free(con->host);
    free(con->password);
    free(con->cipher);
    free(con);

    WSACleanup();
}

bool spice_connection_check(struct spice_connection *con)
{
    if (con->socket != INVALID_SOCKET) {
        return true;
    }

    struct addrinfo *addr_list;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    char port_str[16];
    snprintf(port_str, sizeof(port_str), "%u", con->port);

    int result = getaddrinfo(con->host, port_str, &hints, &addr_list);
    if (result != 0) {
        fprintf(stderr, "getaddrinfo failed: %d\n", result);
        return false;
    }

    for (struct addrinfo *addr = addr_list; addr != NULL; addr = addr->ai_next) {
        con->socket = socket(addr->ai_family, addr->ai_socktype, addr->ai_protocol);
        if (con->socket == INVALID_SOCKET) {
            continue;
        }

        result = connect(con->socket, addr->ai_addr, (int) addr->ai_addrlen);
        if (result == SOCKET_ERROR) {
            closesocket(con->socket);
            con->socket = INVALID_SOCKET;
            continue;
        }

        int opt_val = 1;
        setsockopt(con->socket, IPPROTO_TCP, TCP_NODELAY, (const char *) &opt_val, sizeof(opt_val));
        opt_val = RECEIVE_TIMEOUT;
        setsockopt(con->socket, SOL_SOCKET, SO_RCVTIMEO, (const char *) &opt_val, sizeof(opt_val));

        spice_connection_cipher_alloc(con);
        break;
    }

    freeaddrinfo(addr_list);

    return con->socket != INVALID_SOCKET;
}

void spice_connection_change_pass(struct spice_connection *con, const char *password)
{
    free(con->password);
    con->password = password ? strdup(password) : NULL;
    spice_connection_cipher_alloc(con);
}

char *spice_connection_request(struct spice_connection *con, const char *json)
{
    if (!spice_connection_check(con)) {
        return NULL;
    }

    size_t json_len = strlen(json) + 1;
    uint8_t *json_data = malloc(json_len);
    memcpy(json_data, json, json_len);

    if (con->cipher) {
        spice_rc4_crypt(con->cipher, json_data, json_len);
    }

    int send_result = send(con->socket, (const char *) json_data, (int) json_len, 0);
    free(json_data);

    if (send_result == SOCKET_ERROR || send_result < (int) json_len) {
        closesocket(con->socket);
        con->socket = INVALID_SOCKET;
        return NULL;
    }

    uint8_t *receive_data = malloc(RECEIVE_BUFFER_SIZE);
    size_t receive_data_len = 0;
    int receive_result;

    while ((receive_result = recv(con->socket, (char *) &receive_data[receive_data_len], RECEIVE_BUFFER_SIZE - receive_data_len, 0)) > 0) {
        if (receive_data_len + receive_result >= RECEIVE_BUFFER_SIZE) {
            closesocket(con->socket);
            con->socket = INVALID_SOCKET;
            free(receive_data);
            return NULL;
        }

        if (con->cipher) {
            spice_rc4_crypt(con->cipher, &receive_data[receive_data_len], (size_t) receive_result);
        }

        receive_data_len += receive_result;

        if (receive_data[receive_data_len - 1] == 0) {
            break;
        }
    }

    if (receive_data_len > 0) {
        char *result = strdup((const char *) receive_data);
        free(receive_data);
        return result;
    } else {
        closesocket(con->socket);
        con->socket = INVALID_SOCKET;
        free(receive_data);
        return NULL;
    }
}
