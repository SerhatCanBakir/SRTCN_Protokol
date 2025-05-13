#include "../include/secure_codec.h"
#include "../include/crypto.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define MAX_ENCODED_SIZE 512
#define MIN_MSG_LENGTH 5

int srtcn_secure_encode(srtcn_message_t *msg, const uint8_t *aes_key, const uint8_t *hmac_key) {
    // Giriş kontrolü
    if (!msg || !aes_key || !hmac_key || msg->length < MIN_MSG_LENGTH) 
        return -1;

    uint8_t encoded_msg[MAX_ENCODED_SIZE];
    size_t encoded_len = sizeof(encoded_msg);
    
    // Önce mesajı encode et
    if (srtcn_encode(msg, encoded_msg, &encoded_len) != 0)
        return -2;

    // Buffer taşması kontrolü
    if (msg->length + SRTCN_HMAC_SIZE > sizeof(msg->value))
        return -3;

    uint8_t encrypted_payload[MAX_ENCODED_SIZE];
    
    // AES şifreleme
    if (aes_encrypt(encoded_msg + 5, msg->length - 5, aes_key, encrypted_payload) != 0)
        return -4;

    // Şifrelenmiş veriyi kopyala
    memcpy(msg->value, encrypted_payload, msg->length);

    // HMAC hesapla (şifrelenmiş veri üzerinden)
    uint8_t hmac_output[SRTCN_HMAC_SIZE];
    if (srtcn_hmac_sha256(hmac_key, 32, msg->value, msg->length, hmac_output) != 0)
        return -5;

    // HMAC'i ekle
    memcpy(msg->value + msg->length, hmac_output, SRTCN_HMAC_SIZE);
    msg->length += SRTCN_HMAC_SIZE;

    return 0;
}

int srtcn_secure_decode(srtcn_message_t *in_msg, const uint8_t *aes_key, 
                       const uint8_t *hmac_key, srtcn_message_t *out_msg) {
    // Giriş kontrolü
    if (!in_msg || !aes_key || !hmac_key || !out_msg || 
        in_msg->length < SRTCN_HMAC_SIZE)
        return -1;

    size_t encrypted_len = in_msg->length - SRTCN_HMAC_SIZE;

    // HMAC doğrulama
    uint8_t hmac_output[SRTCN_HMAC_SIZE];
    if (srtcn_hmac_sha256(hmac_key, 32, in_msg->value, encrypted_len, hmac_output) != 0)
        return -2;

    if (memcmp(hmac_output, in_msg->value + encrypted_len, SRTCN_HMAC_SIZE) != 0)
        return -3; // HMAC uyuşmazlığı

    // Şifre çözme
    uint8_t decrypted_payload[MAX_ENCODED_SIZE];
    if (aes_decrypt(in_msg->value, encrypted_len, aes_key, decrypted_payload) != 0)
        return -4;

    // Çıktıyı hazırla
    out_msg->type = in_msg->type;
    out_msg->massage_id = in_msg->massage_id;
    out_msg->length = encrypted_len;
    memcpy(out_msg->value, decrypted_payload, encrypted_len);

    return 0;
}