#include "im_err.h"

const char* im_strerror(im_err_t err)
{
    switch (err)
    {
        /* ========== OK ========== */
    case IM_ERR_OK: return "Success";

        /* ========== Account 1000+ ========== */
    case IM_ERR_USER_EXISTS: return "User already exists";
    case IM_ERR_USER_NOT_FOUND: return "User not found";
    case IM_ERR_USER_PASSWORD: return "User password incorrect";
    case IM_ERR_USER_ALREADY_ONLINE: return "User already online";
    case IM_ERR_USER_NOT_LOGGED_IN: return "User not logged in";

        /* ========== Friend 2000+ ========== */
    case IM_ERR_FRIEND_EXISTS: return "Friend already exists";
    case IM_ERR_FRIEND_NOT_FOUND: return "Friend not found";
    case IM_ERR_FRIEND_SELF_ADD: return "Cannot add yourself as friend";
    case IM_ERR_NOT_FRIEND: return "Not friend";
    case IM_ERR_NO_FRIENDS: return "You have no friends";

        /* ========== Chat 3000+ ========== */
    case IM_ERR_CHAT_OFFLINE: return "Target user is offline";
    case IM_ERR_CHAT_BLOCKED: return "Chat blocked";
    case IM_ERR_CHAT_CONTENT_INVALID: return "Chat content invalid";

        /* ========== DataBase 4000+ ========== */
    case IM_ERR_DB_FAILED: return "Database operation failed";

        /* ========== Protocol / System 9000+ ========== */
    case IM_ERR_INVALID_ARG: return "Invalid argument";
    case IM_ERR_ALLOC_FAILED: return "Memory allocation failed";
    case IM_ERR_PROTO_ENCODE: return "Protocol encode failed";
    case IM_ERR_PROTO_DECODE: return "Protocol decode failed";
    case IM_ERR_NET_INIT: return "Network initialization failed";
    case IM_ERR_NET_SOCKET_CREATE: return "Socket creation failed";
    case IM_ERR_NET_SOCKET_CONNECT: return "Socket connect failed";
    case IM_ERR_NET_SOCKET_BIND: return "Socket bind failed";
    case IM_ERR_NET_SOCKET_LISTEN: return "Socket listen failed";
    case IM_ERR_NET_SOCKET_ACCEPT: return "Socket accept failed";
    case IM_ERR_NET_SOCKET_SEND: return "Socket send failed";
    case IM_ERR_NET_SOCKET_RECV: return "Socket recv failed";
    case IM_ERR_UNKNOWN: return "Unknown error";

    default: return "Unrecognized error code";
    }
}

