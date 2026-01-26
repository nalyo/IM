#pragma once
#include "im_protocol.h"
#include <client_app.h>

void client_dispatch(client_app_t* app, const im_msg_hdr_t* hdr, const void* body);
