#ifndef SRTCN_PLATFORM_H
#define SRTCN_PLATFORM_H

#include <stdint.h>
#include <stddef.h>

typedef int socket_t;

int platform_init();

void platform_cleanup();

socket_t platform_socket_create();

int platform_socket_bind(socket_t sock,char* ip,size_t len,uint16_t port);

int platform_socket_set_reuseaddr(socket_t sock);

int platform_socket_sendto(socket_t sock,const uint8_t *data,size_t len,const char *ip, uint16_t port);

int platform_socket_recvfrom(socket_t sock,uint8_t *buf,size_t maxlen,const char *out_ip,uint16_t *out_port);

void platform_socket_close(socket_t sock);

uint64_t platform_now_ms();

#endif