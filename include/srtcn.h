#ifndef SRTCN_H
#define SRTCN_H

#include <stdint.h>
#include "crypto.h"
#include "platform.h"
#include "message.h"

typedef struct ackSettings
{
uint8_t sendCount;
uint32_t exp_time;
}AckSettings;
typedef struct sublist
{
    char ip[64];
    uint16_t port;
    struct sublist *next;
} Sublist;

typedef struct srtcn_config
{
    uint8_t AES_KEY[16];
    uint8_t HMAC_KEY[32];
    Sublist *sub;
    AckSettings acksetting;
} srtcn_config_t;


int server_init(
void
);



srtcn_config_t* config_init(
    char* aes_key,
    char* Hmac_key
);

//sock is referace in here 
int start_server(
    socket_t* sock,
    char* ip,
    uint16_t port
);

int closeServer(
    socket_t sock
);

int addAckSettings(
srtcn_config_t* config,
uint8_t sendCount,
uint32_t exp_time
);

int addsub(
srtcn_config_t* config,
char* ipaddr,
uint16_t port
);

int removeSub(
    srtcn_config_t* config,
    char* ipaddr,
    uint16_t port
);
int sendMessage(
    srtcn_config_t* config,
    socket_t sock,
    srtcn_message_t msg
);

srtcn_message_t recvMessage(
    srtcn_config_t* config,
    socket_t sock
);

uint64_t getTime();


#endif