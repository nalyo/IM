#pragma once

typedef enum {
    IM_MAIN_USER = 1,   // 账号相关
    IM_MAIN_FRIEND = 2,   // 好友相关
    IM_MAIN_CHAT = 3,   // 聊天相关
    IM_MAIN_SYSTEM = 4    // 系统 / 心跳 / 踢下线
} im_main_cmd_t;

typedef enum {
    IM_USER_REGISTER = 1,
    IM_USER_LOGIN = 2,
    IM_USER_LOGOUT = 3
} im_user_cmd_t;

typedef enum {
    IM_FRIEND_ADD = 1,
    IM_FRIEND_DELETE = 2,
    IM_FRIEND_LIST = 3
} im_friend_cmd_t;

typedef enum {
    IM_CHAT_SEND = 1,
    IM_CHAT_PUSH = 2,
    IM_CHAT_HISTORY = 3
} im_chat_cmd_t;

typedef enum {
    IM_NET_OK = 1,
    IM_NET_ERROR = 2,
    IM_NET_HEARTBEAT = 3,
    IM_NET_KICK = 4
} im_net_cmd_t;

typedef enum {
    IM_SYS_HELP = 1,
    IM_SYS_QUIT = 2
} im_system_cmd_t;
