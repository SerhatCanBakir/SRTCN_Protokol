#include "../include/srtcn.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int server_init(void)
{
    platform_init();
    return 0;
}

srtcn_config_t *config_init(
    char *aes_key,
    char *hmac_key)
{
    srtcn_config_t *config = malloc(sizeof(srtcn_config_t));
    if (!config)
        return NULL;

    memcpy(config->AES_KEY, aes_key, sizeof(uint8_t)*16); // Sabit uzunluk kullanılmalı
    memcpy(config->HMAC_KEY, hmac_key,sizeof(uint8_t)*32);
    config->sub = NULL; // Varsayılan olarak boş başla
    return config;
}

int start_server(socket_t *sock, const char *ip, uint16_t port) {
    *sock = platform_socket_create();

    if (platform_socket_set_reuseaddr(*sock) != 0) {
        platform_socket_close(*sock);
        return -1;
    }

    if (platform_socket_bind(*sock, ip,strlen(ip), port) != 0) {
        platform_socket_close(*sock);
        return -1;
    }

    return 0;
}

int closeServer(
    socket_t sock)
{
    platform_socket_close(sock);
    platform_cleanup();
    return 0;
}

int addAckSettings(
    srtcn_config_t *config,
    uint8_t sendCount,
    uint32_t exp_time)
{
    if (!config)
        return -1;
    AckSettings ack = {0};
    ack.exp_time = exp_time;
    ack.sendCount = sendCount;
    config->acksetting = ack;
    return 0;
}

int addsub(
    srtcn_config_t *config,
    char *ipaddr,
    uint16_t port)
{
    if (!config || !ipaddr) return -1;

    Sublist *newsub = malloc(sizeof(Sublist));
    if (!newsub) return -1;

    // IP adresi buffer'a kopyalanıyor, taşma önleniyor
    strncpy(newsub->ip, ipaddr, sizeof(newsub->ip) - 1);
    newsub->ip[sizeof(newsub->ip) - 1] = '\0';

    newsub->port = port;
    newsub->next = NULL;

    if (config->sub == NULL)
    {
        config->sub = newsub;
    }
    else
    {
        Sublist *holder = config->sub;
        while (holder->next != NULL)
        {
            holder = holder->next;
        }
        holder->next = newsub;
    }

    return 0;
}

int removeSub(
    srtcn_config_t *config,
    char *ipaddr,
    uint16_t port)
{
    if (!config || !config->sub)
        return -1;

    Sublist *holder = config->sub;

    // Özel durum: ilk eleman eşleşirse
    if (strcmp(holder->ip, ipaddr) == 0 && holder->port == port)
    {
        config->sub = holder->next;
        free(holder);
        return 0;
    }

    while (holder->next && (strcmp(holder->next->ip, ipaddr) || holder->next->port != port))
    {
        holder = holder->next;
    }

    if (!holder->next)
        return -1; // Bulunamadı

    Sublist *toDelete = holder->next;
    holder->next = toDelete->next;
    free(toDelete);
    return 0;
}

int sendMessage(
    srtcn_config_t *config,
    socket_t sock,
    srtcn_message_t msg)
{
    if (!!config)
        return -1;

    if (!srtcn_secure_encode(&msg, config->AES_KEY, config->HMAC_KEY))
        return -1;

    uint8_t buff[SRTCN_MAX_PAYLOAD_SIZE];
    int buffsize = sizeof(buff);

    if (!srtcn_encode(&msg, buff, buffsize))
        return -2;

    Sublist *subs = config->sub;
    while (subs != NULL)
    {
        platform_socket_sendto(sock, buff, buffsize, subs->ip, subs->port);
        subs = subs->next;
    }

    return 0;
}

srtcn_message_t recvMessage(
    srtcn_config_t *config,
    socket_t sock)
{
    srtcn_message_t newMsg = {0}, newMsg2 = {0};
    if (!config)
        return newMsg2;

    uint8_t buff[SRTCN_MAX_PAYLOAD_SIZE];
    char ip[64] = {0};
    uint16_t port = 0;

    int recvsize = platform_socket_recvfrom(sock, buff, SRTCN_MAX_PAYLOAD_SIZE, ip, &port);
    if (recvsize <= 0)
        return newMsg2;

    if (!srtcn_decode(buff, recvsize, &newMsg))
        return newMsg2;

    if (!srtcn_secure_decode(&newMsg, config->AES_KEY, config->HMAC_KEY, &newMsg2))
        return newMsg2;

    return newMsg2;
}

uint64_t getTime()
{
    return platform_now_ms();
}
