#pragma once
#include "managers/user_manager.h"
#include "im_err.h"
#include "dao/friend_dao.h"
#include "utils/list.h"
#include <stdint.h>

typedef struct friend_node {
    char username[IM_MAX_USERNAME];
    struct list_head list;
} friend_node_t;

/* 初始化/释放链表 */
void init_friend_list(struct client_info_t* client);
void free_friend_list(struct client_info_t* client);

/* 好友操作 */
im_err_t add_friend(struct client_info_t* client, const char* friend_name);
im_err_t remove_friend(struct client_info_t* client, const char* friend_name);
friend_node_t* find_friend(struct client_info_t* client, const char* friend_name);

/* 获取好友列表（包含在线状态） */
im_err_t get_friend_list(struct client_info_t* client, im_friend_list_t** out_list, size_t* out_size);

/* 好友在线状态 */
int is_friend_online(const char* username);
