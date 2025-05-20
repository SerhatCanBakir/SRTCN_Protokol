#include "platform.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <stdio.h>

int platform_init() {
    // Linux'ta init gerekmez
    return 0;
}

socket_t platform_socket_create() {
    int s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    return (socket_t)s;
}

int platform_socket_bind(socket_t sock, char *ip, size_t len, uint16_t port) {
    struct sockaddr_in addr = {0};

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &addr.sin_addr) != 1) {
        return -1;
    }

    return bind(sock, (struct sockaddr *)&addr, sizeof(addr));
}

int platform_socket_sendto(socket_t sock, const uint8_t *data, size_t len,
                           const char *ip, uint16_t port) {
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &addr.sin_addr) != 1) {
        return -1;
    }

    ssize_t sent_bytes = sendto(sock, data, len, 0,
                                (struct sockaddr *)&addr, sizeof(addr));
    return (sent_bytes == -1) ? -1 : sent_bytes;
}

int platform_socket_recvfrom(socket_t sock, uint8_t *buf, size_t buf_len, char *out_ip, uint16_t *out_port) {
    struct sockaddr_in from;
    socklen_t fromlen = sizeof(from);
    int ret = recvfrom(sock, buf, buf_len, 0, (struct sockaddr *)&from, &fromlen);
    if (ret >= 0) {
        inet_ntop(AF_INET, &from.sin_addr, out_ip, INET_ADDRSTRLEN);
        *out_port = ntohs(from.sin_port);
    }
    return ret;
}

void platform_socket_close(socket_t sock) {
    close(sock);
}

void platform_cleanup() {
    // Linux'ta cleanup gerekmez
}

uint64_t platform_now_ms() {
    struct timespec ts;
    clock_gettime(_POSIX_MONOTONIC_CLOCK, &ts);
    return ((uint64_t)ts.tv_sec * 1000) + (ts.tv_nsec / 1000000);
}
