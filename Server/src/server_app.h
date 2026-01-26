#pragma once
#include "platform/im_socket.h"
#include "im_err.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct {
        im_sock_t listen_fd;
        int running;
    } server_app_t;

    int server_app_start(server_app_t* app, uint16_t port);
    void server_app_stop(server_app_t* app);

#ifdef __cplusplus
}
#endif
