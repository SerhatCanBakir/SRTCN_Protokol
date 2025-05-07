#include "../include/crypto.h"
#include "../externa_libs/hmac_sha256.h"
#include "../externa_libs/aes.h"




int srtcn_hmac_sha256(
    const uint8_t *key, size_t key_len,
    const uint8_t *data, size_t data_len,
    uint8_t *out_hmac)
{
    if (!key || !data || !out_hmac)
        return -1;

    hmac_sha256(key, key_len, data, data_len, out_hmac, SRTCN_HMAC_SIZE);
    return 0;
}

//TODO :: DO THAT MORE SECURE !!!!
int aes_encrypt(const uint8_t *input, size_t input_len, const uint8_t *key, uint8_t *output)
{
    aes_encrypt(input, input_len, key, output);
    return 0;
}

int aes_decrypt(const uint8_t *input, size_t input_len, const uint8_t *key, uint8_t *output)
{
    aes_decrypt(input, input_len, key, output);
    return 0;
}
