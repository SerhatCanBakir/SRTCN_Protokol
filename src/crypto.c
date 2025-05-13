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
    // Giriş kontrolü
    if (!input || !key || !output || input_len == 0)
        return -1;

    // Sabit boyutlu buffer kullanımı
    uint8_t padded_input[MAX_BUFFER_SIZE];
    size_t padded_len = input_len;
    uint8_t pad_value = AES_BLOCK_SIZE - (input_len % AES_BLOCK_SIZE);

    // Buffer taşması kontrolü
    if (input_len + pad_value > MAX_BUFFER_SIZE)
        return -2;

    memcpy(padded_input, input, input_len);
    
    // Padding ekle
    if (pad_value != AES_BLOCK_SIZE) {
        padded_len += pad_value;
        memset(padded_input + input_len, pad_value, pad_value);
    }

    // Şifreleme
    struct AES_ctx ctx;
    uint8_t iv[AES_BLOCK_SIZE] = {0}; // Basit IV
    AES_init_ctx_iv(&ctx, key, iv);
    AES_CBC_encrypt_buffer(&ctx, padded_input, padded_len);
    
    memcpy(output, padded_input, padded_len);
    return 0;
}

int aes_decrypt(const uint8_t *input, size_t input_len,
               const uint8_t *key, uint8_t *output) {
    // Giriş kontrolü
    if (!input || !key || !output || 
        input_len == 0 || input_len % AES_BLOCK_SIZE != 0 ||
        input_len > MAX_BUFFER_SIZE)
        return -1;

    // Sabit boyutlu buffer kullanımı
    uint8_t temp_output[MAX_BUFFER_SIZE];
    
    memcpy(temp_output, input, input_len);
    
    // Şifre çözme
    struct AES_ctx ctx;
    uint8_t iv[AES_BLOCK_SIZE] = {0}; // IV (encode ile aynı olmalı)
    AES_init_ctx_iv(&ctx, key, iv);
    AES_CBC_decrypt_buffer(&ctx, temp_output, input_len);
    
    // Padding kontrolü (basit)
    uint8_t pad_size = temp_output[input_len - 1];
    if (pad_size > AES_BLOCK_SIZE) {
        return -2; // Geçersiz padding
    }
    
    memcpy(output, temp_output, input_len - pad_size);
    return 0;
}