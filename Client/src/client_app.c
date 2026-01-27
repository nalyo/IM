#include "client_app.h"
#include "client_socket.h"
#include "im_protocol.h"
#include "client_input.h"
#include <stdio.h>
#include <string.h>
#include "send/send.h"
#include "platform/im_socket.h"
#include "platform/im_thread.h"
#include "client_dispatch.h"
#include "client_plugin_loader.h"


static void print_prompt(client_app_t* app)
{
    printf("\r[%s]> ", app->username);
    fflush(stdout);
}

static void* client_send_thread(void* arg)
{
    client_app_t* app = (client_app_t*)arg;
    char line[IM_MAX_PAYLOAD];

    print_prompt(app); // 显示初始提示符
    while (app->running) {
        if (fgets(line, sizeof(line), stdin) == NULL)
            break;

        /* 去掉结尾换行 */
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n')
            line[len - 1] = '\0';

        client_handle_input(app, line);
        print_prompt(app); // 处理完输入后再显示提示符
    }
    return NULL;
}

static void* client_recv_thread(void* arg)
{
    client_app_t* app = (client_app_t*)arg;
    char buf[sizeof(im_msg_hdr_t) + IM_MAX_PAYLOAD];

    while (app->running) {
        int n = im_socket_recv(app->sock, buf, sizeof(buf));
        if (n <= 0)
            break;

        im_msg_hdr_t hdr;
        const void* body;

        if (im_decode(buf, n, &hdr, &body) != IM_ERR_OK)
            continue;

        // 清除当前输入行
        printf("\r");                 // 回到行首
        printf("\x1b[K");             // ANSI 清除行

        // 打印消息
        client_dispatch(app, &hdr, body);

        // 重新打印提示符
        print_prompt(app);
    }
    return NULL;
}


int client_app_start(client_app_t* app,
    const char* server_ip,
    uint16_t port)
{
    if (!app || !server_ip)
        return IM_ERR_INVALID_ARG;

    memset(app, 0, sizeof(*app));
    app->running = 1;

    SAFE_STRNCPY(app->username,
        sizeof(app->username),
        "unknown");

    if (im_net_init() != IM_ERR_OK)
        return IM_ERR_NET_INIT;

    app->sock = client_connect(server_ip, port);
    if (!IM_SOCK_VALID(app->sock))
        return IM_ERR_NET_SOCKET_CONNECT;
    
    app->g_plugin_cb.send_message = client_send_packet;
    // 动态加载插件，跨平台
    load_all_plugins("plugins", app);

    im_thread_t tid_send, tid_recv;
    im_thread_create(&tid_send, client_send_thread, app);
    im_thread_create(&tid_recv, client_recv_thread, app);

    im_thread_detach(tid_send);
    im_thread_detach(tid_recv);

    return IM_ERR_OK;
}


void client_app_stop(client_app_t* app)
{
    if (!app) return;
    app->running = 0;
    im_socket_close(app->sock);
    im_net_cleanup();
}
