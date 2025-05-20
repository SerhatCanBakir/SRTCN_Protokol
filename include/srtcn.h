#ifndef SRTCN_H
#define SRTCN_H

#include <stdint.h>
#include "crypto.h"
#include "platform.h"
#include "message.h"

typedef struct sublist_node sublist_node;

typedef struct {
    uint8_t massage_try_time;
    uint32_t massage_time_exp;
} srtcn_massage_info;

struct sublist_node {
    char sub_ip[64];
    uint16_t sub_port;
    sublist_node *next;
};

typedef struct {
    uint8_t AES_KEY[16];
    uint8_t HMAC_KEY[32];
    uint64_t device_id;
    srtcn_massage_info msg_info;
    sublist_node* firstsub;
} srtcn_config;

typedef struct {
    socket_t sock;
    char ip[64];  
    uint16_t port;
} srtcn_server_t;



#endif