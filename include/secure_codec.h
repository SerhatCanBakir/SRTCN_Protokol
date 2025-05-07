#ifndef SECURE_CODEC_H
#define SECURE_CODEC_H

#include <stdint.h>
#include <stddef.h>
#include "message.h"  // srtcn_message_t
#include "crypto.h"   // AES ve HMAC fonksiyonlarını burada kullanacağız

// Güvenli encode fonksiyonu: Mesajı şifreleyip çıkartır
int srtcn_secure_encode(
    const srtcn_message_t *msg,
    const uint8_t *aes_key,
    const uint8_t *hmac_key,
    uint8_t *out_buf,
    size_t *out_len
);

// Güvenli decode fonksiyonu: Şifreli mesajı çözer
int srtcn_secure_decode(
    const uint8_t *in_buf,
    size_t in_len,
    const uint8_t *aes_key,
    const uint8_t *hmac_key,
    srtcn_message_t *out_msg
);

#endif
