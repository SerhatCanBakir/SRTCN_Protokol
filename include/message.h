#ifndef SRTCN_MESSAGE_H
#define SRTCN_MESSAGE_H

#include <stdint.h>
#include <stddef.h>
#include "endian_utils.h"
#include "secure_codec.h"

#define SRTCN_MAX_PAYLOAD_SIZE 256
#define SRTCN_TYPE_DATA 0
#define SRTCN_TYPE_ACK 1
#define SRTCN_TYPE_HEARTBEAT 2 
typedef struct srtcn_message_t
{
    uint16_t massage_id;
    uint8_t type;
    uint16_t length;
    uint8_t value[SRTCN_MAX_PAYLOAD_SIZE];
} srtcn_message_t;

int srtcn_encode(const srtcn_message_t *msg, uint8_t *out_buf, size_t *out_len);
int srtcn_decode(const uint8_t *in_buff, size_t in_len, srtcn_message_t *out_msg);

#endif