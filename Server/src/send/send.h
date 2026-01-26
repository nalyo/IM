#pragma once
#include "im_protocol.h"
#include "server_app.h"

void server_send_packet(im_sock_t fd,
    uint16_t main_cmd,
    uint16_t sub_cmd,
    uint16_t flag,
    uint16_t err_code,
    const void* body,
    uint16_t body_len);
