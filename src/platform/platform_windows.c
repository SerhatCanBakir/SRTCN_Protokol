#include "platform.h"

#include <winsock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <stdio.h>

#pragma comment(lib,"ws2_32.lib")

int platform_init() {
    WSADATA wsa;
    return WSAStartup(MAKEWORD(2, 2), &wsa);
}

socket_t platform_socket_create() {
    SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    return (socket_t)s;
}

int platform_socket_bind(socket_t sock, const char* ip, uint16_t port) {
    struct sockaddr_in addr = {0};

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &addr.sin_addr) != 1) {
        // Geçersiz IP adresi
        return -1;
    }
    return bind((SOCKET)sock, (struct sockaddr*)&addr, sizeof(addr));
}

int platform_socket_set_reuseaddr(socket_t sock) {
    int optval = 1;
    return setsockopt((SOCKET)sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&optval, sizeof(optval));
}

int platform_socket_sendto(socket_t sock, const uint8_t* data, size_t len, 
                          const char* ip, uint16_t port) {
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &addr.sin_addr) != 1) {
        // Geçersiz IP
        return -1;
    }

    int sent_bytes = sendto((SOCKET)sock, (const char*)data, (int)len, 0,
                            (struct sockaddr*)&addr, sizeof(addr));
    if (sent_bytes == SOCKET_ERROR) {
        return -1; // Gönderme hatası
    }
    return sent_bytes;
}

int platform_socket_recvfrom(socket_t sock, uint8_t* buf, size_t buf_len, char* out_ip, uint16_t* out_port) {
    struct sockaddr_in from;
    int fromlen = sizeof(from);

    int ret = recvfrom((SOCKET)sock, (char*)buf, (int)buf_len, 0, (struct sockaddr*)&from, &fromlen);
    if (ret >= 0) {
        if (inet_ntop(AF_INET, &from.sin_addr, out_ip, 64) == NULL) {
            // IP dönüşümü başarısız
            return -1;
        }
        *out_port = ntohs(from.sin_port);
    }
    return ret;
}

void platform_socket_close(socket_t sock) {
    closesocket((SOCKET)sock);
}

void platform_cleanup() {
    WSACleanup();
}

uint64_t platform_now_ms() {
    return GetTickCount64();
}
