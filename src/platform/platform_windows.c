#include "../include/platform.h"

#include <winsock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <stdio.h>

#pragma comment(lib,"ws2_32.lib")

int platform_init(){
    WSADATA wsa;
    return WSAStartup(MAKEWORD(2,2),&wsa);
}

socket_t platform_socket_create(){
    SOCKET s = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    return (socket_t)s;
}

int platform_socket_bind(socket_t sock,char* ip,size_t len,uint16_t port){
  struct sockaddr_in addr={0};

  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  inet_pton(AF_INET,ip,&addr.sin_addr);
  return bind(sock,(struct sockaddr*)&addr,sizeof(addr));
    return 0;
}

int platform_socket_sendto(socket_t sock,const uint8_t *data,size_t len,const char *ip,uint16_t port){
    struct sockaddr_in addr={0};
    addr.sin_family=AF_INET;
    addr.sin_port= htons(port);
    inet_pton(AF_INET,ip,&addr.sin_addr);
return sendto((SOCKET)sock,(const char*)data,(int)len,0,(struct sockaddr*)&addr,sizeof(addr));
}

int platform_socket_recvfrom(socket_t sock,uint8_t buf,size_t buf_len,char out_ip,uint16_t *out_port){
    struct sockaddr_in from;
    int fromlen = sizeof(from);
    int ret = recvfrom((SOCKET)sock, (char *)buf, (int)buf_len, 0, (struct sockaddr*)&from, &fromlen);
    if (ret >= 0) {
        inet_ntop(AF_INET, &from.sin_addr, out_ip, 64);
        *out_port = ntohs(from.sin_port);
    }
    return ret;
}

void platform_socket_close(socket_t sock) {
    closesocket((SOCKET)sock);
}

uint64_t platform_now_ms() {
    return GetTickCount64(); 
}