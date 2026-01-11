#include "spice-sdvxio/spice_rc4.h"

void spice_rc4_init(struct spice_rc4 *rc4, const uint8_t *key, size_t key_size)
{
    rc4->a = 0;
    rc4->b = 0;

    for (size_t i = 0; i < 256; i++) {
        rc4->s_box[i] = (uint8_t) i;
    }

    if (!key_size) {
        return;
    }

    size_t j = 0;
    for (size_t i = 0; i < 256; i++) {
        j = (j + rc4->s_box[i] + key[i % key_size]) % 256;

        uint8_t tmp = rc4->s_box[i];
        rc4->s_box[i] = rc4->s_box[j];
        rc4->s_box[j] = tmp;
    }
}

void spice_rc4_crypt(struct spice_rc4 *rc4, uint8_t *data, size_t size)
{
    for (size_t pos = 0; pos < size; pos++) {
        rc4->a = (rc4->a + 1) % 256;
        rc4->b = (rc4->b + rc4->s_box[rc4->a]) % 256;

        uint8_t tmp = rc4->s_box[rc4->a];
        rc4->s_box[rc4->a] = rc4->s_box[rc4->b];
        rc4->s_box[rc4->b] = tmp;

        data[pos] ^= rc4->s_box[(rc4->s_box[rc4->a] + rc4->s_box[rc4->b]) % 256];
    }
}
