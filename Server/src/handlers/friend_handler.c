#include "handlers.h"
#include "send/send.h"
#include "utils/im_string.h"
#include "managers/user_manager.h"
#include "managers/friend_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void handle_friend(client_info_t* client,
    const im_msg_hdr_t* hdr,
    const void* body)
{
    if (!client || !hdr)
        return;

    switch (hdr->sub_cmd) {

        /* ================= 添加好友 ================= */
    case IM_FRIEND_ADD: {
        const char* friend_name = (const char*)body;
        // 调用 Manager 层
        im_err_t err;
        if (client->logged_in)
            err = add_friend(client, friend_name);
        else
            err = IM_ERR_USER_NOT_LOGGED_IN;
        const char* strerror = im_strerror(err);
        printf("[FRIEND] add: %s, result: %s \n", client->username, strerror);

        // 填充返回 payload
        im_friend_info_t info;
        memset(&info, 0, sizeof(info));
        SAFE_STRNCPY(info.username, sizeof(info.username), friend_name);

        server_send_packet(
            client->fd,
            IM_MAIN_FRIEND,
            IM_FRIEND_ADD,
            IM_FLAG_RESPONSE,
            err,
            (err == IM_ERR_OK) ? &info : NULL,
            sizeof(info)
        );
        break;
    }

                      /* ================= 删除好友 ================= */
    case IM_FRIEND_DELETE: {
        const char* friend_name = (const char*)body;

        // 调用 Manager 层
        im_err_t err;
        if (client->logged_in)
            err = remove_friend(client, friend_name);
        else
            err = IM_ERR_USER_NOT_LOGGED_IN;
        const char* strerror = im_strerror(err);
        printf("[FRIEND] remove: %s, result: %s \n", client->username, strerror);

        // 填充返回 payload
        im_friend_info_t info;
        memset(&info, 0, sizeof(info));
        SAFE_STRNCPY(info.username, sizeof(info.username), friend_name);

        server_send_packet(
            client->fd,
            IM_MAIN_FRIEND,
            IM_FRIEND_DELETE,
            IM_FLAG_RESPONSE,
            err,
            (err == IM_ERR_OK) ? &info : NULL,
            sizeof(info)
        );
        break;
    }

                         /* ================= 列出好友 ================= */
    case IM_FRIEND_LIST: {

        im_friend_list_t* flist = NULL;
        size_t buf_size = 0;

        im_err_t err;
        if (client->logged_in)
            err = get_friend_list(client, &flist, &buf_size);
        else
            err = IM_ERR_USER_NOT_LOGGED_IN;
        // 调用 Manager 层获取好友列表（包含在线状态）
        const char* strerror = im_strerror(err);
        printf("[FRIEND] list: %s, result: %s \n", client->username, strerror);

        // 返回给客户端
        server_send_packet(
            client->fd,
            IM_MAIN_FRIEND,
            IM_FRIEND_LIST,
            IM_FLAG_RESPONSE,
            err,
            flist,
            buf_size
        );

        free(flist); // Handler 层释放 Manager 分配的内存
        break;
    }

    default:
        printf("[FRIEND] unknown friend sub_cmd: %u\n", hdr->sub_cmd);
        break;
    }
}
