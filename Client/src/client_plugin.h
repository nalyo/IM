#ifndef CLIENT_PLUGIN_H
#define CLIENT_PLUGIN_H

#include "im_protocol.h"
#include "platform/im_socket.h"
#include <stdint.h>


typedef struct client_app client_app_t;
// 插件回调结构体
typedef struct client_plugin_cb {
    int (*send_message)(im_sock_t fd,
        uint16_t main_cmd,
        uint16_t sub_cmd,
        const void* body,
        uint16_t body_len);
}client_plugin_cb_t;

typedef struct client_plugin {
    const char* name;

    void (*on_message)(client_app_t* app, const im_msg_hdr_t* hdr, const void* body);
    void (*on_input)(client_app_t* app, const char* input);
    void (*on_start)(client_app_t* app);
    void (*on_stop)(client_app_t* app);

    struct client_plugin* next;
}client_plugin_t;

void client_plugin_register(client_plugin_t* plugin);

void client_plugin_dispatch_message(client_app_t* app, const im_msg_hdr_t* hdr, const void* body);
void client_plugin_dispatch_input(client_app_t* app, const char* input);

#endif
