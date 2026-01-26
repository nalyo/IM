#pragma once
#include "managers/user_manager.h"
#include "dao/chat_dao.h"

/* 发送消息（存库 + 在线推送） */
im_err_t chat_send_message(client_info_t* from_client, const char* to_username, const char* msg);

/* 获取聊天记录，按时间顺序返回最近 n 条 */
im_err_t chat_get_history(const char* user1, const char* user2, im_chat_msg_t* out_msgs, int max_count);
