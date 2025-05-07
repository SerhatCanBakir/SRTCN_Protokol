#include "../include/secure_codec.h"
#include "../include/crypto.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>

int srtcn_secure_encode(
    const srtcn_message_t *msg,
    const uint8_t *aes_key,
    const uint8_t *hmac_key,
    uint8_t *out_buf,
    size_t *out_len)
{
    if (!msg || !aes_key || !hmac_key || !out_buf || !out_len)
        return -1;

    uint8_t encoded_msg[512];
    size_t encoded_len = 0;

    int ret = srtcn_encode(msg, encoded_msg, &encoded_len);
    if (ret != 0)
        return ret;

    uint8_t encrypted_payload[512];
    aes_encrypt(encoded_msg + 5, msg->length, aes_key, encrypted_payload);

    memcpy(out_buf, encrypted_payload, msg->length);

    uint8_t hmac_output[SRTCN_HMAC_SIZE];
    srtcn_hmac_sha256(hmac_key, 32, out_buf, msg->length, hmac_output);

    memcpy(out_buf + msg->length, hmac_output, SRTCN_HMAC_SIZE);

    *out_len = msg->length + SRTCN_HMAC_SIZE;

    return 0;
}

int srtcn_secure_decode(
    const uint8_t *in_buf,
    size_t in_len,
    const uint8_t *aes_key,
    const uint8_t *hmac_key,
    srtcn_message_t *out_msg)
    {
        if (!in_buf || in_len < (SRTCN_MAX_PAYLOAD_SIZE + SRTCN_HMAC_SIZE) || !out_msg)
        return -1;

    // HMAC'i kontrol et
    uint8_t hmac_output[SRTCN_HMAC_SIZE];
    srtcn_hmac_sha256(hmac_key, 32, in_buf, in_len - SRTCN_HMAC_SIZE, hmac_output);

    if (memcmp(hmac_output, in_buf + in_len - SRTCN_HMAC_SIZE, SRTCN_HMAC_SIZE) != 0)
        return -2;  // HMAC doğrulaması başarısız

    // HMAC doğrulandı, şifreyi çöz
    uint8_t decrypted_payload[512];
    aes_decrypt(in_buf, in_len - SRTCN_HMAC_SIZE, aes_key, decrypted_payload);

    // Mesajı çöz
    return srtcn_decode(decrypted_payload, in_len - SRTCN_HMAC_SIZE, out_msg);
    }