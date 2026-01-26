#include "handlers.h"
#include <stdio.h>
#include <string.h>
#include "im_protocol.h"  // im_ok_t

void handle_friend(client_app_t* app,
    const im_msg_hdr_t* hdr,
    const void* body)
{
    if (!hdr) return;

    /* 只处理响应 */
    if ((hdr->flags & IM_FLAG_RESPONSE) == 0)
        return;

    switch (hdr->sub_cmd) {

    case IM_FRIEND_ADD: {
        if (hdr->err_code == IM_ERR_OK) {
            const im_friend_info_t* info = (const im_friend_info_t*)body;
            printf("[FRIEND] Add friend success: %s\n", info->username);
        }
        else {
            printf("[FRIEND] Add friend failed: %s\n", im_strerror(hdr->err_code));
        }
        break;
    }

    case IM_FRIEND_DELETE: {
        if (hdr->err_code == IM_ERR_OK) {
            const im_friend_info_t* info = (const im_friend_info_t*)body;
            printf("[FRIEND] Delete friend success: %s\n", info->username);
        }
        else {
            printf("[FRIEND] Delete friend failed: %s\n", im_strerror(hdr->err_code));
        }
        break;
    }

    case IM_FRIEND_LIST: {
        if (hdr->err_code == IM_ERR_OK) {
            const im_friend_list_t* flist = (const im_friend_list_t*)body;
            printf("[FRIEND] Friend list (%u):\n", flist->count);
            for (uint16_t i = 0; i < flist->count; i++) {
                printf("  %s [%s]\n",
                    flist->friends[i].username,
                    flist->friends[i].online ? "在线" : "离线");
            }
        }
        else {
            printf("[FRIEND] Failed to get friend list: %s\n",
                im_strerror(hdr->err_code));
        }
        break;
    }

    default:
        printf("[WARN] Unknown friend sub_cmd: %u\n", hdr->sub_cmd);
        break;
    }
}

