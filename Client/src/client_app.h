#pragma once
#include "platform/im_socket.h"
#include "im_err.h"

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct {
        im_sock_t sock;
        volatile int running;
        uint16_t last_cmd;           // 最近发送的命令
        int logged_in;
        char username[32];
    } client_app_t;

    int client_app_start(client_app_t* app, const char* server_ip, uint16_t port);
    void client_app_stop(client_app_t* app);

#ifdef __cplusplus
}
#endif
