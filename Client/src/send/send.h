#pragma once
#include <stdint.h>
#include "client_app.h"

int client_send_packet(im_sock_t fd,
    uint16_t main_cmd,
    uint16_t sub_cmd,
    const void* body,
    uint16_t body_len);