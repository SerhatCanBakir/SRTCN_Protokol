#ifndef SRTCN_ENDIAN_UTILS_H
#define SRTCN_ENDIAN_UTILS_H

#include <stdint.h>

uint16_t strcn_htons(uint16_t val);
uint16_t strcn_ntohs(uint16_t val);
uint32_t strcn_htonl(uint32_t val);
uint32_t strcn_ntohl(uint32_t val);

#endif