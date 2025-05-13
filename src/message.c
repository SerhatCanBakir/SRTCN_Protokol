#include "../include/message.h"
#include "../include/endian_utils.h"
#include <string.h>
#include <stdint.h>
#include <stddef.h>

int srtcn_encode(const srtcn_message_t *msg, uint8_t *out_buf, size_t *out_len)
{
    if (!msg || !out_buf || !out_len)
    {
        return -1;
    }
    if (msg->length > SRTCN_MAX_PAYLOAD_SIZE)
    {
        return -2;
    }
    memcpy(out_buf + 5, msg->value, msg->length);

    *out_len = 5 + msg->length;
    return 0;
}

int srtcn_decode(const uint8_t *in_buff, size_t in_len, srtcn_message_t *out_msg)
{
    if (!in_buff || in_len < 5 || !out_msg)
        return -1;

    out_msg->type = in_buff[0];
    out_msg->massage_id = strcn_ntohs(*(uint16_t *)(in_buff + 1));
    out_msg->length = strcn_ntohs(*(uint16_t *)(in_buff + 3));

    if (out_msg->length > SRTCN_MAX_PAYLOAD_SIZE || in_len < 5 + out_msg->length)
        return -2;
    memcpy(out_msg->value, in_buff + 5, out_msg->length);
    return 0;
}