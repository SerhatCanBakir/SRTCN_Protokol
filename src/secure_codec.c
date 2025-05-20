#include "../include/secure_codec.h"
#include "../include/crypto.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define MAX_ENCODED_SIZE 512
#define MIN_MSG_LENGTH 5

int srtcn_secure_encode(srtcn_message_t *msg, const uint8_t *aes_key, const uint8_t *hmac_key)
{
    // Data validation
    if (!msg || !aes_key || !hmac_key || msg->length < MIN_MSG_LENGTH)
        return -1;

    // HMAC_ENCRYPTİON
    uint8_t HMAC_TOKEN[SRTCN_HMAC_SIZE];
    srtcn_hmac_sha256(hmac_key, sizeof(uint8_t) * 32, msg->value, msg->length, &HMAC_TOKEN);
    memcpy(msg->value + msg->length, &HMAC_TOKEN, sizeof(HMAC_TOKEN));
    msg->length += SRTCN_HMAC_SIZE;

    // AES ENCRYPTİON
    uint8_t aes_buffer[MAX_ENCODED_SIZE];
    int aes_length = aes_encrypt(msg->value, msg->length, aes_key, aes_buffer);
    memcpy(msg->value, aes_buffer, aes_length);
    msg->length = aes_length;

    return 0;
}

int srtcn_secure_decode(srtcn_message_t *in_msg, const uint8_t *aes_key,
                        const uint8_t *hmac_key, srtcn_message_t *out_msg)
{
    if (!in_msg || !aes_key || !hmac_key || !out_msg || in_msg->length < SRTCN_HMAC_SIZE)
        return -1;

    uint8_t decrypted[SRTCN_MAX_PAYLOAD_SIZE];
    int decrypted_len = aes_decrypt(in_msg->value, in_msg->length, aes_key, decrypted);
    if (decrypted_len < 0) return -1;

    // Remove padding if necessary here or ensure aes_decrypt returns unpadded length

    // Copy decrypted payload
    memcpy(out_msg->value, decrypted, decrypted_len);
    out_msg->length = decrypted_len;

    size_t payload_len_without_hmac = out_msg->length - SRTCN_HMAC_SIZE;

    
    uint8_t hmacOutput[SRTCN_HMAC_SIZE];
    srtcn_hmac_sha256(hmac_key, 32, out_msg->value, payload_len_without_hmac, hmacOutput);

    
    uint8_t *appended_hmac = out_msg->value + payload_len_without_hmac;

    // HMAC verification
    if (memcmp(hmacOutput, appended_hmac, SRTCN_HMAC_SIZE) != 0) {
        return -2; // HMAC verification failed
    }

    
    out_msg->length = payload_len_without_hmac;

    return 0;
}
