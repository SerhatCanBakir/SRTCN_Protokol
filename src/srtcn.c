// HELLO MADAFAKA
#include "../include/crypto.h"
#include "../include/endian_utils.h"
#include "../include/message.h"
#include "../include/platform.h"
#include "../include/secure_codec.h"
#include "../include/srtcn.h"

#include <string.h>
#include <stdio.h>

static srtcn_config_t g_config;
static socket_t g_sock = -1;

int srtcn_init(const srtcn_config_t *config)
{
    if (!config || !config->device_id || !config->hmac_key)
        return -1;

    memcpy(&g_config, config, sizeof(srtcn_config_t));

    if (platform_init != 0)
        return -2;

    g_sock = platform_socket_create();
    if (g_sock < 0)
        return -3;

    int res = platform_socket_bind(g_sock, &config->sender_ip, sizeof(config->sender_ip), config->bind_port);
    if (res != 0)
        return res;

    return 0;
}

void srtcn_deinit()
{
    if (g_sock >= 0)
    {
        platform_socket_close(g_sock);
            g_sock = -1;
    }
}

int srtcn_send(const char *ip, uint16_t port, const uint8_t *payload, size_t len)
{
    if (!ip || !payload || len == 0 || len > SRTCN_MAX_PAYLOAD)
        return -1;

    uint8_t encoded_buf[SRTCN_MAX_PACKET];
    size_t encoded_len = sizeof(encoded_buf);

    if (srtcn_secure_encode(payload, len, encoded_buf, &encoded_len, (const uint8_t *)g_config.device_id, strlen(g_config.device_id), g_config.hmac_key, g_config.hmac_key_length) != 0)
        return -2;

    int sent = platform_socket_sendto(g_sock, encoded_buf, encoded_len, ip, port);
    return (sent == (int)encoded_len) ? 0 : -3;
}

int srtcn_recv(srtcn_packet_t *out_packet)
{
    if (!out_packet)
        return -1;

    uint8_t buffer[SRTCN_MAX_PACKET];
    int ret = platform_socket_recvfrom(g_sock, buffer, sizeof(buffer),
                                       out_packet->sender_ip, &out_packet->sender_port);

    if (ret < 0)
        return -2;

    uint8_t decoded[SRTCN_MAX_PAYLOAD];
    size_t decoded_len = sizeof(decoded);

    if (secure_codec_decrypt(buffer, ret, decoded, &decoded_len,
                             (const uint8_t *)g_config.device_id, strlen(g_config.device_id),
                             g_config.hmac_key, g_config.hmac_key_length) != 0)
    {
        return -3;
    }

    if (decoded_len > SRTCN_MAX_PAYLOAD)
        return -4;

    memcpy(out_packet->data, decoded, decoded_len);
    out_packet->len = decoded_len;

    return 0;
}

uint64_t srtcn_now_ms(void){
    return platform_now_ms();
}