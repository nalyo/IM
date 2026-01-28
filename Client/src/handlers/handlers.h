#pragma once
#include "im_protocol.h"
#include "client_app.h"

void handle_user(client_app_t* app, const im_msg_hdr_t* hdr, const void* body);
void handle_friend(client_app_t* app, const im_msg_hdr_t* hdr, const void* body);
void handle_chat(const im_msg_hdr_t* hdr, const void* body);
void handle_system(uint16_t sub_cmd, const void* body);
