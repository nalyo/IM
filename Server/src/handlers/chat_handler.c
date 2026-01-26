#include "handlers.h"
#include "send/send.h"
#include "managers/user_manager.h"
#include "managers/chat_manager.h"
#include <stdio.h>

void handle_chat(client_info_t* client,
    const im_msg_hdr_t* hdr,
    const void* body)
{
    if (!client || !hdr || !body)
        return;

    if (hdr->flags & IM_FLAG_RESPONSE)
        return;

    switch (hdr->sub_cmd) {

        /* 发送消息 */
    case IM_CHAT_SEND: {
        if (!client->logged_in) {
            server_send_packet(client->fd, IM_MAIN_CHAT, IM_CHAT_SEND,
                IM_FLAG_RESPONSE, IM_ERR_USER_NOT_LOGGED_IN,
                NULL, 0);
            return;
        }

        const im_chat_msg_t* chat = (const im_chat_msg_t*)body;

        im_err_t err = chat_send_message(client, chat->to, chat->message);
        const char* strerror = im_strerror(err);
        printf("[CHAT] send: %s -> %s : %s, result: %s \n", client->username, chat->to, chat->message, strerror);

        server_send_packet(client->fd, IM_MAIN_CHAT, IM_CHAT_SEND,
            IM_FLAG_RESPONSE, err,
            NULL, 0);
        break;
    }

                     /* 拉取历史聊天记录 */
    case IM_CHAT_HISTORY: {
        im_chat_msg_t msgs[10]; // 最多返回 10 条
        int n = chat_get_history(client->username, (const char*)body, msgs, 10);
        const char* strerror = im_strerror(n != 0 ? IM_ERR_OK : IM_ERR_DB_FAILED);
        printf("[CHAT] history: %s <-> %s : %d messages, result: %s \n", client->username, (const char*)body, n, strerror);

        server_send_packet(client->fd, IM_MAIN_CHAT, IM_CHAT_HISTORY,
            IM_FLAG_RESPONSE, IM_ERR_OK,
            msgs, sizeof(im_chat_msg_t) * n);
        break;
    }

    default:
        printf("[CHAT] unknown chat sub_cmd: %u\n", hdr->sub_cmd);
        break;
    }
}


