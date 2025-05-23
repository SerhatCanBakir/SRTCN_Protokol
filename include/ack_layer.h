#ifndef ACK_LAYER_H
#define ACK_LAYER_H

#include "message.h"
enum msgStatus
{
    IDLE,
    SENT,
    ACKED,
    FAIL
};

typedef struct
{
    srtcn_message_t msg;
    msgStatus status;
    uint64_t last_sent_time;
    uint8_t retry_count;
    char *dest_ip;
    uint16_t dest_port;
} srtcn_transmisson_entry;





#endif