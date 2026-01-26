#pragma once
#include "im_protocol.h"
#include "platform/im_socket.h"
#include "managers/user_manager.h"

void handle_user(client_info_t* client, const im_msg_hdr_t* hdr, const void* body);
void handle_chat(client_info_t* client, const im_msg_hdr_t* hdr, const void* body);
void handle_friend(client_info_t* client, const im_msg_hdr_t* hdr, const void* body);
//void handle_system(im_sock_t fd, uint16_t sub_cmd, const void* body);
