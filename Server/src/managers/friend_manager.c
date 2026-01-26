#include "managers/friend_manager.h"
#include "managers/user_manager.h"
#include "dao/friend_dao.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <dao/user_dao.h>
#include <stddef.h>


/* 简单全局锁保护链表 + 数据库操作，保证线程安全 */
static pthread_mutex_t friend_lock = PTHREAD_MUTEX_INITIALIZER;

/* 初始化好友链表 */
void init_friend_list(struct client_info_t* client)
{
    if (!client) return;
    INIT_LIST_HEAD(&client->friend_list);
}

/* 释放好友链表 */
void free_friend_list(struct client_info_t* client)
{
    if (!client) return;

    struct list_head* pos, * tmp;
    list_for_each_safe(pos, tmp, &client->friend_list) {
        friend_node_t* node = list_entry(pos, friend_node_t, list);
        list_del(&node->list);
        free(node);
    }
    INIT_LIST_HEAD(&client->friend_list);
}

/* 查找好友节点 */
friend_node_t* find_friend(struct client_info_t* client, const char* friend_name)
{
    if (!client || !friend_name) return NULL;

    struct list_head* pos;
    list_for_each(pos, &client->friend_list) {
        friend_node_t* node = list_entry(pos, friend_node_t, list);
        if (strcmp(node->username, friend_name) == 0)
            return node;
    }
    return NULL;
}

/* 添加好友（同步数据库 + 内存链表） */
im_err_t add_friend(struct client_info_t* client, const char* friend_name)
{
    if (!client || !friend_name)
        return IM_ERR_FRIEND_NOT_FOUND;

    if (strcmp(client->username, friend_name) == 0)
        return IM_ERR_FRIEND_SELF_ADD;

    pthread_mutex_lock(&friend_lock);

    if (find_friend(client, friend_name)) {
        pthread_mutex_unlock(&friend_lock);
        return IM_ERR_FRIEND_EXISTS;
    }

    // 查询好友的 user_id
    user_record_t user;
    if (dao_user_find_by_name(friend_name, &user) != 0) {
        pthread_mutex_unlock(&friend_lock);
        return IM_ERR_FRIEND_NOT_FOUND;
    }

    // 更新数据库
    if (friend_add(client->user_id, user.id) != 0) {
        pthread_mutex_unlock(&friend_lock);
        return IM_ERR_DB_FAILED;
    }

    // 创建内存节点，只保存用户名
    friend_node_t* node = malloc(sizeof(friend_node_t));
    if (!node) {
        pthread_mutex_unlock(&friend_lock);
        return IM_ERR_ALLOC_FAILED;
    }

    SAFE_STRNCPY(node->username, sizeof(node->username), friend_name);
    INIT_LIST_HEAD(&node->list);
    list_add_tail(&node->list, &client->friend_list);

    pthread_mutex_unlock(&friend_lock);
    return IM_ERR_OK;
}


/* 删除好友（同步数据库 + 内存链表） */
im_err_t remove_friend(struct client_info_t* client, const char* friend_name)
{
    if (!client || !friend_name)
        return IM_ERR_FRIEND_NOT_FOUND;

    pthread_mutex_lock(&friend_lock);

    // 查找内存链表节点
    friend_node_t* node = find_friend(client, friend_name);
    if (!node) {
        pthread_mutex_unlock(&friend_lock);
        return IM_ERR_FRIEND_NOT_FOUND;
    }

    // 查询好友 user_id（不分配临时 client_info_t）
    user_record_t user;
    if (dao_user_find_by_name(friend_name, &user) != 0) {
        pthread_mutex_unlock(&friend_lock);
        return IM_ERR_FRIEND_NOT_FOUND;
    }

    // 删除数据库记录
    if (friend_delete(client->user_id, user.id) != 0) {
        pthread_mutex_unlock(&friend_lock);
        return IM_ERR_DB_FAILED;
    }

    // 删除内存链表节点
    list_del(&node->list);
    free(node);

    pthread_mutex_unlock(&friend_lock);
    return IM_ERR_OK;
}


/* 好友在线状态 */
int is_friend_online(const char* username)
{
    if (!username) return 0;
    return (find_online_client(username) != NULL);
}

/* 内部回调，用于从 DAO 填充 Manager 内存链表 */
static void friend_list_cb(const char* name, int online, void* arg)
{
    client_info_t* client = (client_info_t*)arg;
    friend_node_t* node = malloc(sizeof(friend_node_t));
    if (!node) return;

    SAFE_STRNCPY(node->username, sizeof(node->username), name);
    INIT_LIST_HEAD(&node->list);
    list_add_tail(&node->list, &client->friend_list);
}

/* 获取好友列表，返回动态分配内存（Handler 层负责 free） */
im_err_t get_friend_list(client_info_t* client, im_friend_list_t** out_list, size_t* out_size)
{
    if (!client || !out_list || !out_size) return IM_ERR_INVALID_ARG;

    pthread_mutex_lock(&friend_lock);

    // 先清空内存链表，然后从数据库填充
    free_friend_list(client);
    friend_list(client->user_id, friend_list_cb, client);

    // 计算数量
    uint16_t count = 0;
    struct list_head* pos;
    list_for_each(pos, &client->friend_list) count++;

    if (count == 0) {
        *out_list = NULL;
        *out_size = 0;
        pthread_mutex_unlock(&friend_lock);
        return IM_ERR_NO_FRIENDS;
    }

    // 分配返回内存
    size_t buf_size = sizeof(im_friend_list_t) + count * sizeof(((im_friend_list_t*)0)->friends[0]);
    im_friend_list_t* flist = malloc(buf_size);
    if (!flist) {
        pthread_mutex_unlock(&friend_lock);
        return IM_ERR_ALLOC_FAILED;
    }
    flist->count = count;

    uint16_t idx = 0;
    list_for_each(pos, &client->friend_list) {
        friend_node_t* node = list_entry(pos, friend_node_t, list);
        SAFE_STRNCPY(flist->friends[idx].username, sizeof(node->username), node->username);
        flist->friends[idx].online = is_friend_online(node->username) ? 1 : 0;
        idx++;
    }

    *out_list = flist;
    *out_size = buf_size;

    pthread_mutex_unlock(&friend_lock);
    return IM_ERR_OK;
}
