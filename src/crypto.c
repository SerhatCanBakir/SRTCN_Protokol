#include "../include/crypto.h"
#include "../externa_libs/hmac_sha256.h"
#include "../externa_libs/aes.h"
#include <string.h>
#include <stdlib.h>

#define AES_BLOCK_SIZE 16
#define MAX_BUFFER_SIZE 512

int srtcn_hmac_sha256(const uint8_t *key, size_t key_len, 
                     const uint8_t *data, size_t data_len, 
                     uint8_t *out_hmac) {
    // Giriş kontrolü
    if (!key || !data || !out_hmac || key_len == 0 || data_len == 0)
        return -1;

    // HMAC fonksiyonunuzun gerçek prototipine uygun çağrı
    hmac_sha256(key, key_len, data, data_len, out_hmac, SRTCN_HMAC_SIZE);
    return 0;
}

int aes_encrypt(const uint8_t *input, size_t input_len, 
                const uint8_t *key, uint8_t *output) {
    if (!input || !key || !output || input_len == 0)
        return -1;

    uint8_t padded_input[MAX_BUFFER_SIZE];
    uint8_t iv[AES_BLOCK_SIZE];
    size_t padded_len;
    uint8_t pad_value;

    // Padding
    pad_value = AES_BLOCK_SIZE - (input_len % AES_BLOCK_SIZE);
    if (pad_value == 0) pad_value = AES_BLOCK_SIZE;
    padded_len = input_len + pad_value;

    if (padded_len > MAX_BUFFER_SIZE)
        return -2;

    memcpy(padded_input, input, input_len);
    memset(padded_input + input_len, pad_value, pad_value);

    // Rastgele IV üret (örneğin Windows için)
    for (int i = 0; i < AES_BLOCK_SIZE; i++)
        iv[i] = rand() % 256;

    // CBC başlat
    struct AES_ctx ctx;
    AES_init_ctx_iv(&ctx, key, iv);
    AES_CBC_encrypt_buffer(&ctx, padded_input, padded_len);

    // IV + Ciphertext olarak output'a yaz
    memcpy(output, iv, AES_BLOCK_SIZE);
    memcpy(output + AES_BLOCK_SIZE, padded_input, padded_len);

    return AES_BLOCK_SIZE + padded_len;
}

int aes_decrypt(const uint8_t *input, size_t input_len,
                const uint8_t *key, uint8_t *output) {
    if (!input || !key || !output || 
        input_len <= AES_BLOCK_SIZE || 
        (input_len - AES_BLOCK_SIZE) % AES_BLOCK_SIZE != 0 ||
        input_len > MAX_BUFFER_SIZE)
        return -1;

    const uint8_t *iv = input;
    const uint8_t *ciphertext = input + AES_BLOCK_SIZE;
    size_t ciphertext_len = input_len - AES_BLOCK_SIZE;

    uint8_t temp_output[MAX_BUFFER_SIZE];
    memcpy(temp_output, ciphertext, ciphertext_len);

    struct AES_ctx ctx;
    AES_init_ctx_iv(&ctx, key, iv);
    AES_CBC_decrypt_buffer(&ctx, temp_output, ciphertext_len);

    // PKCS#7 padding doğrulama
    uint8_t pad_value = temp_output[ciphertext_len - 1];
    if (pad_value == 0 || pad_value > AES_BLOCK_SIZE)
        return -2;

    for (size_t i = 0; i < pad_value; ++i) {
        if (temp_output[ciphertext_len - 1 - i] != pad_value)
            return -3; // Hatalı padding
    }

    size_t plain_len = ciphertext_len - pad_value;
    memcpy(output, temp_output, plain_len);
    return (int)plain_len;
}
