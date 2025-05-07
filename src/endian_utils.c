#include "../include/endian_utils.h"

#if defined(_WIN32) || defined(_WIN64)
#include <WinSock2.h>
#else
#include <arpa/inet.h>
#endif

uint16_t strcn_htons(uint16_t val)
{
    return htons(val);
}

uint16_t strcn_ntohs(uint16_t val)
{
    return ntohs(val);
}
uint32_t strcn_htonl(uint32_t val)
{
    return htonl(val);
}
uint32_t strcn_ntohl(uint32_t val)
{
    return ntohl(val);
}