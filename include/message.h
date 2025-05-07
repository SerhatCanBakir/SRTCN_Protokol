#ifndef SRTCN_MESSAGE_H
#define SRTCN_MESSAGE_H

#include <stdint.h>
#include <stddef.h>
#include "./endian_utils.h"

#define SRTCN_MAX_PAYLOAD_SIZE 256
#define SRTCN_TYPE_DATA 1
#define SRTCN_TYPE_ACK 2


//TLV STRUCT
typedef struct
{
    uint8_t type;
    uint16_t message_id;
    uint16_t length;
    uint8_t Payload[SRTCN_MAX_PAYLOAD_SIZE];
} srtcn_message_t;

int srtcn_encode(const srtcn_message_t *msg, uint8_t *out_buf,size_t *out_len);

int srtcn_decode(const uint8_t *in_buff ,size_t in_len ,srtcn_message_t *out_msg);

#endif