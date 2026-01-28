#include "cmd_common.h"
#include "utils/parse.h"
#include "send/send.h"
#include "im_protocol.h"
#include "log.h"
#include <stdio.h>
#include <time.h>

static int cmd_chat(client_app_t* app, const char* args)
{
    char to[IM_MAX_USERNAME], msg[IM_MAX_MESSAGE];
    const char* p = args;

    if (read_token(&p, to, sizeof(to)) < 0 ||
        read_rest(&p, msg, sizeof(msg)) < 0) {
        log_info("usage: chat <to> <message>");
        return -1;
    }

    im_chat_msg_t chat = { 0 };
    SAFE_STRNCPY(chat.to, sizeof(chat.to), to);
    SAFE_STRNCPY(chat.message, sizeof(chat.message), msg);
    chat.timestamp = (int)time(NULL);

    return client_send_packet(app->sock,
        IM_MAIN_CHAT,
        IM_CHAT_SEND,
        &chat,
        sizeof(chat));
}

/* ====================== 查询历史消息 ====================== */
static int cmd_history(client_app_t* app, const char* args)
{
    char target[IM_MAX_USERNAME];
    const char* p = args;

    if (read_token(&p, target, sizeof(target)) < 0) {
        log_info("usage: history <username>");
        return -1;
    }

    // 构造请求包，body 就是目标用户名
    return client_send_packet(app->sock,
        IM_MAIN_CHAT,
        IM_CHAT_HISTORY,   // 新的子命令
        target,
        strlen(target) + 1);
}

const cmd_entry_t g_chat_cmds[] = {
    { "chat", cmd_chat, IM_CHAT_SEND },
    { "history", cmd_history, IM_CHAT_HISTORY }
};
const size_t g_chat_cmds_count =
sizeof(g_chat_cmds) / sizeof(g_chat_cmds[0]);
