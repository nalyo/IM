#pragma once
#include "im_protocol.h"

int message_save(int from_id, int to_id, const char* msg);
int message_get_history(const char* user1,
    const char* user2,
    im_chat_msg_t* out_msgs,
    int max_count);