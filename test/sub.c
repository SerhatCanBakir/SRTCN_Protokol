#include <stdio.h>
#include <string.h>
#include <windows.h> // sleep için

#include "srtcn.h"
#include <stdio.h>

void print_config(srtcn_config_t *config) {
    if (!config) {
        printf("Config pointer is NULL.\n");
        return;
    }

    printf("========== SRTCN CONFIG ==========\n");
    printf("AES_KEY    : %s\n", config->AES_KEY);
    printf("HMAC_KEY   : %s\n", config->HMAC_KEY);
    printf("AckSettings:\n");
    printf("  SendCount: %u\n", config->acksetting.sendCount);
    printf("  Expiry   : %u ms\n", config->acksetting.exp_time);

    printf("Subscribers:\n");
    Sublist *cur = config->sub;
    int i = 0;
    while (cur != NULL) {
        printf("  [%d] IP: %s, Port: %u\n", i++, cur->ip, cur->port);
        cur = cur->next;
    }
    if (i == 0) {
        printf("  (no subscribers)\n");
    }

    printf("==================================\n");
}

int main(int argc, char **argv)
{
    // 1. Init configuration
    char *aeskey = "random_aes_key";
    char *hmackey = "random_hmac_key";

    srtcn_config_t *config = config_init(aeskey, hmackey);

    server_init();
    printf("server_init worked\n");

    socket_t sock;
    if (start_server(&sock, "127.0.0.1", 3001) != 0)
    {
        fprintf(stderr, "Server failed to start.\n");
        return 1;
    }
    printf("start_server worked\n");

    addAckSettings(config, 3, 300); // ACK için 3 tekrar, 300ms expire
    printf("add_ack_settings worked\n");

    // Burada bu sunucuya cevap göndermesi beklenen diğer sunuculara sub eklenmeli (örneğin publisher)
    if (addsub(config, "127.0.0.1", 3000) != 0)
    {
        fprintf(stderr, "addsub failed.\n");
        return 1;
    }
    printf("addSub worked\n");
   print_config(config);
   
   while (1)
   {
    srtcn_message_t recvmsg = recvMessage(config, sock);
    
    printf("GELEN MESAJ:\n");
    printf("message_id: %d\n", recvmsg.massage_id);
    printf("type: %d\n", recvmsg.type);
    printf("length: %d\n", recvmsg.length);
    printf("value: %s\n", (char *)recvmsg.value);
    
    if (recvmsg.type == SRTCN_TYPE_DATA)
    {
        srtcn_message_t ack;
        ack.massage_id = recvmsg.massage_id; // aynı ID ile cevap ver
        ack.type = SRTCN_TYPE_ACK;
        ack.length = 0; // payload yok
        memset(ack.value, 0, sizeof(ack.value));
        
        if (sendMessage(config, sock, ack) != 0)
        {
                fprintf(stderr, "ACK sendMessage failed\n");
            }
            else
            {
                printf("ACK gönderildi\n");
            }
        }
        
        Sleep(1); // test amaçlı 1 saniye bekle
    }

    
    closeServer(sock);
    return 0;
}
