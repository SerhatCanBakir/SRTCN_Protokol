#include <stdio.h>
#include <string.h>
#include <windows.h> 

#include "srtcn.h"

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
    // 1. Initialize configuration
    char *aeskey = "random_aes_key";
    char *hmackey = "random_hmac_key";

    srtcn_config_t *config = config_init(aeskey, hmackey);

    server_init();
    printf("server_init worked\n");

    socket_t sock;
    if (start_server(&sock, "127.0.0.1", 3000) != 0)
    {
        fprintf(stderr, "Server failed to start.\n");
        return 1;
    }
    printf("start_server worked\n");

    addAckSettings(config, 3, 300);
    printf("add_ack_settings worked\n");

    if (addsub(config, "127.0.0.1", 3001) != 0)
    {
        fprintf(stderr, "addsub failed.\n");
        return 1;
    }
    printf("addSub worked\n");

    srtcn_message_t msg;
  print_config(config);
    while (1)
    {
        msg.massage_id = 1;
        msg.type = SRTCN_TYPE_DATA;

        const char *msgstr = "bu bir mesajdır";
        strncpy((char *)msg.value, msgstr, sizeof(msg.value) - 1);
        msg.value[sizeof(msg.value) - 1] = '\0'; // null-terminator
        msg.length = strlen((char *)msg.value);

        int holder;
        if ((holder = sendMessage(config, sock, msg)) != 0)
        {
            fprintf(stderr, "sendMessage failed status : %d\n",holder);
        }
        else
        {
            printf("sendMessage worked\n");
        }

        srtcn_message_t newmsg = recvMessage(config, sock);

        printf(
            "message_id: %d\nmessage_type: %d\nmessage_length: %d\nmessage_payload: %s\n",
            newmsg.massage_id, newmsg.type, newmsg.length, (char *)newmsg.value);

       Sleep(1);
    }

    closeServer(sock);
    return 0;
}
