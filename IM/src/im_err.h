#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

    typedef enum {
        IM_ERR_OK = 0,

        /* ========== Account 1000+ ========== */
        IM_ERR_USER_EXISTS = 1001,
        IM_ERR_USER_NOT_FOUND,
        IM_ERR_USER_PASSWORD,
        IM_ERR_USER_ALREADY_ONLINE,
        IM_ERR_USER_NOT_LOGGED_IN,

        /* ========== Friend 2000+ ========== */
        IM_ERR_FRIEND_EXISTS = 2001,
        IM_ERR_FRIEND_NOT_FOUND,
        IM_ERR_FRIEND_SELF_ADD,
        IM_ERR_NOT_FRIEND,
        IM_ERR_NO_FRIENDS,

        /* ========== Chat 3000+ ========== */
        IM_ERR_CHAT_OFFLINE = 3001,
        IM_ERR_CHAT_BLOCKED,
        IM_ERR_CHAT_CONTENT_INVALID,

        /* ========== DataBase 3000+ ========== */
        IM_ERR_DB_FAILED = 4001,

        /* ========== Protocol / System 9000+ ========== */
        IM_ERR_INVALID_ARG = 9001,
        IM_ERR_ALLOC_FAILED,
        IM_ERR_PROTO_ENCODE,
        IM_ERR_PROTO_DECODE,
        IM_ERR_NET_INIT,              // 网络初始化失败
        IM_ERR_NET_SOCKET_CREATE,     // Socket 创建失败
        IM_ERR_NET_SOCKET_CONNECT,    // 连接失败
        IM_ERR_NET_SOCKET_BIND,       // 绑定失败
        IM_ERR_NET_SOCKET_LISTEN,     // listen 失败
        IM_ERR_NET_SOCKET_ACCEPT,     // accept 失败
        IM_ERR_NET_SOCKET_SEND,       // 发送失败
        IM_ERR_NET_SOCKET_RECV,        // 接收失败
        IM_ERR_UNKNOWN
    } im_err_t;

    // ----------------------------
    // 可选：打印错误字符串
    // ----------------------------
    const char* im_strerror(im_err_t module_err); // 具体实现可以根据模块判断

#ifdef __cplusplus
}
#endif
