#ifndef SECURE_CODEC_H
#define SECURE_CODEC_H

#include <stdint.h>
#include <stddef.h>
#include "message.h"
#include "crypto.h"

typedef struct srtcn_message_t srtcn_message_t;


int srtcn_secure_encode(
    srtcn_message_t *msg,
    const uint8_t aes_key[16],  // AES-128 için
    const uint8_t hmac_key[32]  // HMAC-SHA256 için
);

int srtcn_secure_decode(
    srtcn_message_t *in_msg,
    const uint8_t aes_key[16],
    const uint8_t hmac_key[32],
    srtcn_message_t *out_msg
);

#endif