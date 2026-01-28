#pragma once
#include "platform/im_socket.h"
#include "utils/im_string.h"
#include "utils/im_list.h"
#include "im_protocol.h"
#include "im_err.h"
#include <stdint.h>

#define MAX_ONLINE_USERS 128

/* ================= 在线用户信息 ================= */
typedef struct client_info_t {
    im_sock_t fd;
    int logged_in;

    int user_id;
    char username[IM_MAX_USERNAME];
    char password[IM_MAX_PASSWORD]; // 可选，方便登录验证或密码修改

    struct list_head friend_list;   // 好友缓存链表
} client_info_t;

/* ================= 生命周期 ================= */
void client_manager_init(void);
void client_manager_cleanup(void);

/* ================= 用户注册 / 登录 / 登出 ================= */
im_err_t client_register(const char* username, const char* password);
im_err_t client_login(client_info_t* client, const char* username, const char* password);
im_err_t client_logout(client_info_t* client);

/* ================= 在线缓存管理 ================= */
int client_online(client_info_t* client);
void client_offline(client_info_t* client);

/* ================= 查询 ================= */
client_info_t* find_online_client(const char* username);
client_info_t* find_client_by_fd(im_sock_t fd);
