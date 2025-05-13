#include "../include/srtcn.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static srtcn_config global_config;
static srtcn_server_t global_server;

int srtcn_init(srtcn_config config)
{
    // AES_KEY kontrolü ve kopyalama
    if (config.AES_KEY == NULL) {
        printf("Error: AES_KEY is required\n");
        return 1;
    }
    memcpy(global_config.AES_KEY, config.AES_KEY, sizeof(global_config.AES_KEY));

    // HMAC_KEY kontrolü ve kopyalama
    if (config.HMAC_KEY == NULL) {
        printf("Error: HMAC_KEY is required\n");
        return 1;
    }
    memcpy(global_config.HMAC_KEY, config.HMAC_KEY, sizeof(global_config.HMAC_KEY));

    // Device ID kopyalama
    global_config.device_id = config.device_id;

    // Mesaj bilgileri kopyalama
    if (config.msg_info.massage_time_exp == 0 || config.msg_info.massage_try_time == 0) {
        printf("Warning: Message info not fully configured\n");
    }
    global_config.msg_info = config.msg_info;

    // Bağlantılı liste başlatıcı
    global_config.firstsub = NULL;

    return 0;
}

int srtcn_server_init(srtcn_server_t server_info)
{
    platform_init();
    
    // Sunucu bilgilerini kopyala
    global_server.port = server_info.port;
    strncpy(global_server.ip, server_info.ip, sizeof(global_server.ip) - 1);
    global_server.ip[sizeof(global_server.ip) - 1] = '\0';

    // Soket oluştur ve bağla
    global_server.sock = platform_socket_create();
    if (platform_socket_bind(global_server.sock, global_server.ip, 
                           (uint16_t)strlen(global_server.ip), global_server.port) != 0) {
        printf("Error: Binding failed\n");
        platform_socket_close(global_server.sock);
        return 1;
    }

    return 0;
}

int srtcn_deinit(void)
{
    // Bağlantılı listeyi temizle
    sublist_node *current = global_config.firstsub;
    while (current != NULL) {
        sublist_node *next = current->next;
        free(current);
        current = next;
    }
    global_config.firstsub = NULL;

    // Soketi kapat
    if (global_server.sock != NULL) {
        platform_socket_close(global_server.sock);
    }
    
    platform_cleanup();
    return 0;
}

int srtcn_add_sub(const char sub_ip[64], uint16_t port)
{
    if (sub_ip == NULL) {
        return 1;
    }

    sublist_node *node = malloc(sizeof(sublist_node));
    if (node == NULL) {
        return 1;
    }

    strncpy(node->sub_ip, sub_ip, sizeof(node->sub_ip) - 1);
    node->sub_ip[sizeof(node->sub_ip) - 1] = '\0';
    node->sub_port = port;
    node->next = NULL;

    if (global_config.firstsub == NULL) {
        global_config.firstsub = node;
    } else {
        sublist_node *current = global_config.firstsub;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = node;
    }

    return 0;
}

int srtcn_send(srtcn_message_t msg) {
    uint8_t out_buff[2048]; // Increased buffer size
    size_t out_buff_size = sizeof(out_buff);

    if (srtcn_secure_encode(&msg, global_config.AES_KEY, global_config.HMAC_KEY) != 0) {
        return 1;
    }

    if (srtcn_encode(&msg, out_buff, &out_buff_size) != 0) {
        return 1;
    }

    sublist_node *node = global_config.firstsub;
    while (node != NULL) {
        if (platform_socket_sendto(global_server.sock, out_buff, out_buff_size,
                                 node->sub_ip, node->sub_port) < 0) {
            // Log error but continue with other subscribers
            perror("Sendto failed");
        }
        node = node->next;
    }

    return 0;
}

srtcn_message_t srtcn_recv(char ip[64], uint16_t *port)
{
    uint8_t out_buff[512];
    size_t out_buff_len = sizeof(out_buff);
    srtcn_message_t msg_in, msg_out;
    char temp_ip[64];
    uint16_t temp_port;

    // Veriyi al
    if (platform_socket_recvfrom(global_server.sock, out_buff, &out_buff_len, 
                               temp_ip, &temp_port) != 0) {
        memset(&msg_out, 0, sizeof(msg_out));
        return msg_out;
    }

    // IP ve port bilgisini kopyala
    if (ip != NULL) {
        strncpy(ip, temp_ip, 64);
    }
    if (port != NULL) {
        *port = temp_port;
    }

    // Mesajı çöz
    if (srtcn_decode(out_buff, out_buff_len, &msg_in) != 0) {
        memset(&msg_out, 0, sizeof(msg_out));
        return msg_out;
    }

    // Güvenli çözme
    if (srtcn_secure_decode(&msg_in, global_config.AES_KEY, global_config.HMAC_KEY, &msg_out) != 0) {
        memset(&msg_out, 0, sizeof(msg_out));
    }

    return msg_out;
}

uint64_t now_ms(void)
{
    return platform_now_ms();
}