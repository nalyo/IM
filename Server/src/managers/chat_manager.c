#include "chat_manager.h"
#include "dao/chat_dao.h"
#include "dao/user_dao.h"
#include "managers/user_manager.h"
#include "send/send.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

/* 发送消息 */
im_err_t chat_send_message(client_info_t* from_client, const char* to_username, const char* msg)
{
    if (!from_client || !from_client->logged_in || !to_username || !msg)
        return IM_ERR_INVALID_ARG;

    /* ---------------- 保存消息到数据库 ---------------- */
    client_info_t* to_client = find_online_client(to_username);
    int to_id = 0;

    if (to_client) {
        to_id = to_client->user_id;
    }
    else {
        // 对方不在线，也可以查数据库获得 user_id
        user_record_t user;
        if (dao_user_find_by_name(to_username, &user) != 0) {
            return IM_ERR_USER_NOT_FOUND; // 对方不存在
        }
        to_id = user.id;
    }

    if (message_save(from_client->user_id, to_id, msg) != 0) {
        return IM_ERR_DB_FAILED; // 数据库错误
    }

    /* ---------------- 在线推送 ---------------- */
    if (to_client) {
        im_chat_msg_t push;
        memset(&push, 0, sizeof(push));
        SAFE_STRNCPY(push.from, sizeof(push.from), from_client->username);
        SAFE_STRNCPY(push.to, sizeof(push.to), to_username);
        SAFE_STRNCPY(push.message, sizeof(push.message), msg);
        push.timestamp = (int)time(NULL);

        server_send_packet(
            to_client->fd,
            IM_MAIN_CHAT,
            IM_CHAT_PUSH,
            IM_FLAG_RESPONSE,
            0,
            &push,
            sizeof(push)
        );
    }

    return 0;
}

/* 获取聊天记录（最近 n 条） */
im_err_t chat_get_history(const char* user1, const char* user2, im_chat_msg_t* out_msgs, int max_count)
{
    if (!user1 || !user2 || !out_msgs || max_count <= 0)
        return IM_ERR_INVALID_ARG;

    return message_get_history(user1, user2, out_msgs, max_count);
}
