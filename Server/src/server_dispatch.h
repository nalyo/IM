#pragma once
#include "im_protocol.h"
#include "platform/im_socket.h"
#include "managers/user_manager.h"

void server_dispatch(client_info_t* client, const im_msg_hdr_t* hdr, const void* body);
