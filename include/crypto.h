#ifndef SRTCN_CRYPTO_H
#define SRTCN_CRYPTO_H

#include <stdint.h>
#include <stddef.h>

#define SRTCN_HMAC_SIZE 32

int aes_encrypt(const uint8_t *input,size_t input_len,const uint8_t *key,uint8_t *output);
int aes_decrypt(const uint8_t *input,size_t input_len,const uint8_t *key,uint8_t *output);



int srtcn_hmac_sha256(
const uint8_t *key, size_t key_len,
const uint8_t *data , size_t data_len,
uint8_t *out_hmac
);



#endif