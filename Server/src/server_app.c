#include "server_app.h"
#include "server_socket.h"
#include "im_protocol.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "platform/im_socket.h"
#include "platform/im_thread.h"
#include "managers/user_manager.h"
#include "dao/db.h"
#include "server_dispatch.h"

static void* handle_client(void* arg)
{
    client_info_t* client = (client_info_t*)arg;
    char buf[1024];

    while (1) {
        int n = im_socket_recv(client->fd, buf, sizeof(buf));
        if (n <= 0) break;

        im_msg_hdr_t hdr;
        const void* body;
        if (im_decode(buf, n, &hdr, &body) != IM_ERR_OK) continue;

        // 分发
        server_dispatch(client, &hdr, body);
    }

    im_socket_close(client->fd);
    if (client->logged_in) {
        client_logout(client);  // 下线 + 释放好友链表
    }
    else {
        free(client);           // 如果没登录，只释放临时对象
    }
    return NULL;
}


static void* server_accept_thread(void* arg)
{
    server_app_t* app = (server_app_t*)arg;

    while (app->running) {
        client_info_t* client = malloc(sizeof(client_info_t));
        if (!client) return NULL;
        memset(client, 0, sizeof(*client));
        client->fd = server_accept(app->listen_fd);

        if (client->fd < 0) {
            free(client);
            continue;
        }

        im_thread_t tid;
        im_thread_create(&tid, handle_client, client);
        im_thread_detach(tid);
    }
    return NULL;
}

int server_app_start(server_app_t* app, uint16_t port)
{
    if (!app) return IM_ERR_INVALID_ARG;
    app->running = 1;

    if (db_init("./im_server.db") != IM_ERR_OK)
        return IM_ERR_DB_FAILED;
    client_manager_init();

    if (im_net_init() != IM_ERR_OK) return IM_ERR_NET_INIT;

    app->listen_fd = server_listen(port, 5);
    if (app->listen_fd < 0) return IM_ERR_NET_SOCKET_LISTEN;

    im_thread_t tid;
    im_thread_create(&tid, server_accept_thread, app);
    im_thread_detach(tid);

    return IM_ERR_OK;
}

void server_app_stop(server_app_t* app)
{
    if (!app) return;
    app->running = 0;
    im_socket_close(app->listen_fd);
    im_net_cleanup();
    client_manager_cleanup();
    db_close();
}