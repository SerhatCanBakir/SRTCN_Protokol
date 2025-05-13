#include <stdio.h>
#include <string.h>
#include "srtcn.h"

int main(int argc, char **argv)
{
    // 1. Initialize configuration
    srtcn_config newConfig = {0}; // Zero initialize the struct

    // AES key should be exactly 16 bytes for AES-128
    const char aeskey[16] = "MY_SECRET_AES__"; // 16 characters exactly
    memcpy(newConfig.AES_KEY, aeskey, sizeof(newConfig.AES_KEY));

    // HMAC key should be exactly 32 bytes for SHA-256
    const char hmackey[32] = "MY_SECRET_HMAC_KEY_1234567890_";
    memcpy(newConfig.HMAC_KEY, hmackey, sizeof(newConfig.HMAC_KEY));

    newConfig.device_id = 1;

    // Set message info (added missing initialization)
    newConfig.msg_info.massage_try_time = 3;    // Example retry count
    newConfig.msg_info.massage_time_exp = 5000; // 5 second timeout

    // 2. Initialize SRTCN
    if (srtcn_init(newConfig))
    {
        printf("Failed to initialize SRTCN\n");
        return 1;
    }
    printf("init oluştu");

    // 3. Initialize server
    srtcn_server_t newServer = {0};
    strncpy(newServer.ip, "127.0.0.1", sizeof(newServer.ip) - 1);
    newServer.port = 3000; // Fixed typo (was assigning to ip twice)

    if (srtcn_server_init(newServer))
    {
        printf("Failed to initialize server\n");
        srtcn_deinit();
        return 1;
    }
printf("server açıldı");
    // 4. Add subscriber
    if (srtcn_add_sub("127.0.0.1", 3001))
    {
        printf("Failed to add subscriber\n");
    }
printf("sub eklendi");
    // 5. Create and send message
    srtcn_message_t newMessage = {0};
    newMessage.massage_id = 0;
    newMessage.type = 0; // 0x00 = data, 0x01 = ack
    newMessage.length = (uint16_t)strlen("this is a message!!");
    memcpy(newMessage.value, "this is a message!!", newMessage.length);

    if (srtcn_send(newMessage)!=0)
    {
        printf("Failed to send message\n");
    }
printf("mesaj gitti");
    // 6. Receive message
    char sender_ip[64] = {0};
    uint16_t sender_port = 0;
    srtcn_message_t received = srtcn_recv(sender_ip, &sender_port);

    printf("Received from %s:%d\n", sender_ip, sender_port);
    printf("id: %d \ntype: %d \nlength: %d \nvalue: %.*s\n",
           received.massage_id,
           received.type,
           received.length,
           received.length,
           received.value);
printf("mesaj geldi");
    // 7. Cleanup
    srtcn_deinit();

    // Wait for user input before exiting (portable version)
    printf("Press Enter to exit...");
    getchar();

    return 0;
}