#include "handlers.h"
#include "platform/im_time.h"
#include "log.h"
#include <stdio.h>

void handle_chat(const im_msg_hdr_t* hdr, const void* body)
{
    if (!hdr)
        return;

    /* 只处理 server 响应或 push */
    if ((hdr->flags & IM_FLAG_RESPONSE) == 0) {
        log_warn("received unexpected request from server");
        return;
    }

    switch (hdr->sub_cmd) {

        /* --------- 发送消息确认 --------- */
    case IM_CHAT_SEND:
        if (hdr->err_code == IM_ERR_OK) {
            log_info("[CHAT] Message sent successfully.");
        }
        else {
            log_warn("[CHAT] Failed to send message: %s",
                im_strerror(hdr->err_code));
        }
        break;

        /* --------- 推送消息 --------- */
    case IM_CHAT_PUSH:
        if (body) {
            const im_chat_msg_t* chat = (const im_chat_msg_t*)body;
            char time_str[IM_TIME_LEN];
            im_format_time(chat->timestamp, time_str, sizeof(time_str));
            log_info("[%s -> %s] %s: %s",
                chat->from, chat->to, time_str, chat->message);
        }
        break;


        /* --------- 历史消息响应 --------- */
    case IM_CHAT_HISTORY:
        if (body && hdr->len > 0) {
            const im_chat_msg_t* msgs = (const im_chat_msg_t*)body;
            uint16_t count = hdr->len / sizeof(im_chat_msg_t); // 根据 hdr->len 计算条数
            log_info("[CHAT HISTORY] Total %u messages:", count);
            for (uint16_t i = 0; i < count; ++i) {
                const im_chat_msg_t* m = &msgs[i];
                char time_str[IM_TIME_LEN];
                im_format_time(m->timestamp, time_str, sizeof(time_str));
                log_info("[%s -> %s] %s: %s", m->from, m->to, time_str, m->message);
            }
        }
        else {
            log_info("[CHAT HISTORY] No messages.\n");
        }
        break;

    default:
        log_warn("Unknown chat sub_cmd: %u\n", hdr->sub_cmd);
        break;
    }
}
