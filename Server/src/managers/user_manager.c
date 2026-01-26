#include "managers/user_manager.h"
#include "managers/friend_manager.h"
#include "dao/user_dao.h"   // DAO 层操作数据库
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "platform/im_thread.h"

/* ================= 全局在线缓存 ================= */
static client_info_t* g_clients[MAX_ONLINE_USERS];
static int g_client_count = 0;
static im_mutex_t g_client_lock;

/* ================= 生命周期 ================= */
void client_manager_init(void)
{
    g_client_count = 0;
    memset(g_clients, 0, sizeof(g_clients));
    im_mutex_init(&g_client_lock);
}

void client_manager_cleanup(void)
{
    im_mutex_lock(&g_client_lock);

    for (int i = 0; i < g_client_count; ++i) {
        free_friend_list(g_clients[i]);
        free(g_clients[i]);
        g_clients[i] = NULL;
    }
    g_client_count = 0;

    im_mutex_unlock(&g_client_lock);
    im_mutex_destroy(&g_client_lock);
}

/* ================= 用户注册 / 登录 / 登出 ================= */
im_err_t client_register(const char* username, const char* password)
{
    user_record_t tmp;
    if (dao_user_find_by_name(username, &tmp) == 0) {
        return IM_ERR_USER_EXISTS;
    }
    if (dao_user_create(username, password) != 0)
        return IM_ERR_DB_FAILED;
    return IM_ERR_OK;
}

im_err_t client_login(client_info_t* client,
    const char* username,
    const char* password)
{
    if (!client || !username)
        return IM_ERR_INVALID_ARG;

    /* 查询数据库用户 */
    user_record_t user;
    if (dao_user_find_by_name(username, &user) != 0)
        return IM_ERR_USER_NOT_FOUND;

    if (strcmp(user.password, password) != 0)
        return IM_ERR_USER_PASSWORD;

    /* 原地初始化 client_info_t */
    client->user_id = user.id;
    /* 加入在线缓存 */
    if (client_online(client) != IM_ERR_OK) {
        free_friend_list(client);
        return IM_ERR_USER_ALREADY_ONLINE;
    }
    SAFE_STRNCPY(client->username, sizeof(client->username), username);
    SAFE_STRNCPY(client->password, sizeof(client->password), password);

    /* 初始化好友链表 */
    init_friend_list(client);


    return IM_ERR_OK;
}


im_err_t client_logout(client_info_t* client)
{
    if (!client) return IM_ERR_INVALID_ARG;
    client_offline(client);
    free_friend_list(client);
    return IM_ERR_OK;
}

/* ================= 在线缓存管理 ================= */
int client_online(client_info_t* client)
{
    if (!client) return -1;

    im_mutex_lock(&g_client_lock);

    // 防止重复登录
    for (int i = 0; i < g_client_count; ++i) {
        if (g_clients[i]->user_id == client->user_id) {
            im_mutex_unlock(&g_client_lock);
            return -1;
        }
    }

    if (g_client_count >= MAX_ONLINE_USERS) {
        im_mutex_unlock(&g_client_lock);
        return -1;
    }

    g_clients[g_client_count++] = client;
    client->logged_in = 1;
    im_mutex_unlock(&g_client_lock);
    return 0;
}

void client_offline(client_info_t* client)
{
    if (!client) return;
    im_mutex_lock(&g_client_lock);

    for (int i = 0; i < g_client_count; ++i) {
        if (g_clients[i] == client) {
            client->logged_in = 0;
            g_clients[i] = g_clients[g_client_count - 1];
            g_clients[g_client_count - 1] = NULL;
            g_client_count--;
            break;
        }
    }

    im_mutex_unlock(&g_client_lock);
}

/* ================= 查询 ================= */
client_info_t* find_online_client(const char* username)
{
    if (!username) return NULL;
    im_mutex_lock(&g_client_lock);

    for (int i = 0; i < g_client_count; ++i) {
        if (strcmp(g_clients[i]->username, username) == 0) {
            client_info_t* ret = g_clients[i];
            im_mutex_unlock(&g_client_lock);
            return ret;
        }
    }

    im_mutex_unlock(&g_client_lock);
    return NULL;
}

client_info_t* find_client_by_fd(im_sock_t fd)
{
    im_mutex_lock(&g_client_lock);

    for (int i = 0; i < g_client_count; ++i) {
        if (g_clients[i]->fd == fd) {
            client_info_t* ret = g_clients[i];
            im_mutex_unlock(&g_client_lock);
            return ret;
        }
    }

    im_mutex_unlock(&g_client_lock);
    return NULL;
}
