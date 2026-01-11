#ifndef SPICE_RC4_H
#define SPICE_RC4_H

#include <stddef.h>
#include <stdint.h>

struct spice_rc4 {
    uint8_t s_box[256];
    size_t a;
    size_t b;
};

void spice_rc4_init(struct spice_rc4 *rc4, const uint8_t *key, size_t key_size);
void spice_rc4_crypt(struct spice_rc4 *rc4, uint8_t *data, size_t size);

#endif
