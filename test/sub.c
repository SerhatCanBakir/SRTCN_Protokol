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

    // Set message info
    newConfig.msg_info.massage_try_time = 3;    // Example retry count
    newConfig.msg_info.massage_time_exp = 5000; // 5 second timeout

    // 2. Initialize SRTCN
    if (srtcn_init(newConfig))
    {
        fprintf(stderr, "Failed to initialize SRTCN\n");
        return 1;
    }
printf("init yapıldı");
    // 3. Initialize server
    srtcn_server_t newServer = {0};
    strncpy(newServer.ip, "127.0.0.1", sizeof(newServer.ip) - 1);
    newServer.ip[sizeof(newServer.ip) - 1] = '\0'; // Ensure null termination
    newServer.port = 3001;                         // Fixed port assignment

    if (srtcn_server_init(newServer))
    {
        fprintf(stderr, "Failed to initialize server\n");
        srtcn_deinit();
        return 1;
    }
printf("server açıldı");
    // 4. Add publisher as subscriber (reverse of publisher example)
    if (srtcn_add_sub("127.0.0.1", 3000))
    {
        fprintf(stderr, "Failed to add publisher as subscriber\n");
    }
printf("sub eklendi");
    // 5. Receive message
    char sender_ip[64] = "127.0.0.1";
    uint16_t sender_port = 3000; 
    srtcn_message_t received = srtcn_recv(sender_ip, &sender_port);
printf("mesaj gitti");
    printf("Received message from %s:%d\n", sender_ip, sender_port);
    printf("Message ID: %d\nType: %d\nLength: %d\nValue: %.*s\n",
           received.massage_id,
           received.type,
           received.length,
           received.length,
           received.value);

    // 6. Send ACK
    srtcn_message_t ackMessage = {0};
    ackMessage.massage_id = received.massage_id; // Echo back the received ID
    ackMessage.type = 1;                         // ACK type
    ackMessage.length = sizeof(received.massage_id);
    memcpy(ackMessage.value, &received.massage_id, sizeof(received.massage_id));

    if (srtcn_send(ackMessage)!=0)
    {
        fprintf(stderr, "Failed to send ACK\n");
    }
    else
    {
        printf("Sent ACK for message ID %d\n", received.massage_id);
    }

    // 7. Cleanup
    srtcn_deinit();

    return 0;
}