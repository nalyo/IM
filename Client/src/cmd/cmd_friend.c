#include "cmd_common.h"
#include "utils/parse.h"
#include "send/send.h"
#include "im_protocol.h"
#include "log.h"
#include <stdio.h>

/* --------- 添加好友 --------- */
static int cmd_friend_add(client_app_t* app, const char* args)
{
    char name[IM_MAX_USERNAME];
    if (read_rest(&args, name, sizeof(name)) < 0) {
        log_info("usage: fadd <name>");
        return -1;
    }

    im_friend_info_t friend_payload = { 0 };
    SAFE_STRNCPY(friend_payload.username, sizeof(friend_payload.username), name);

    return client_send_packet(app->sock,
        IM_MAIN_FRIEND,
        IM_FRIEND_ADD,
        &friend_payload,
        sizeof(friend_payload));
}

/* --------- 删除好友 --------- */
static int cmd_friend_del(client_app_t* app, const char* args)
{
    char name[IM_MAX_USERNAME];
    if (read_rest(&args, name, sizeof(name)) < 0) {
        log_info("usage: fdel <name>");
        return -1;
    }

    im_friend_info_t friend_payload = { 0 };
    SAFE_STRNCPY(friend_payload.username, sizeof(friend_payload.username), name);

    return client_send_packet(app->sock,
        IM_MAIN_FRIEND,
        IM_FRIEND_DELETE,
        &friend_payload,
        sizeof(friend_payload));
}

/* --------- 获取好友列表 --------- */
static int cmd_friend_list(client_app_t* app, const char* args)
{
    if (args && *args) {
        log_info("usage: flist");
        return -1;
    }

    return client_send_packet(app->sock,
        IM_MAIN_FRIEND,
        IM_FRIEND_LIST,
        NULL,
        0);
}

/* --------- 命令表 --------- */
const cmd_entry_t g_friend_cmds[] = {
    { "fadd", cmd_friend_add, IM_FRIEND_ADD },
    { "fdel", cmd_friend_del, IM_FRIEND_DELETE },
    { "flist", cmd_friend_list, IM_FRIEND_LIST },
};
const size_t g_friend_cmds_count =
sizeof(g_friend_cmds) / sizeof(g_friend_cmds[0]);
