// ABSULUTE CINEMA
#ifndef SRTCN_H
#define SRTCN_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif



#define SRTCN_MAX_PAYLOAD 1024
#define SRTCN_MAX_PACKET 1200


typedef struct {
uint8_t data[SRTCN_MAX_PAYLOAD];
size_t len;
char sender_ip[64];
uint16_t sender_port;
}srtcn_packet_t;

typedef struct{
    const char* device_id;
    const uint8_t hmac_key;
    size_t hmac_key_length;
    uint16_t bind_port;
    char sender_ip[64];
}srtcn_config_t;

int srtcn_init(const srtcn_config_t* config);

void srtcn_deinit();

int srtcn_send(const char* ip,uint16_t port, const uint8_t* payload ,size_t len);

int srtcn_recv(srtcn_packet_t* out_packet);

uint64_t srtcn_now_ms(void)

#ifdef __cplusplus
}
#endif

#endif