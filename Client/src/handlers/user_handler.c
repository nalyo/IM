#include "handlers.h"
#include <stdio.h>
#include <string.h>
#include "client_app.h"   // client_app_t
#include "platform/im_tools.h"

void handle_account(client_app_t* app,
    const im_msg_hdr_t* hdr,
    const void* body)
{
    if (!app || !hdr)
        return;

    /* ---------------- 先处理错误 ---------------- */
    if ((hdr->flags & IM_FLAG_RESPONSE) &&
        hdr->err_code != IM_ERR_OK)
    {
        printf("[ACCOUNT ERROR] %s\n",
            im_strerror(hdr->err_code));
        return;
    }

    /* ---------------- 成功分支 ---------------- */
    switch (hdr->sub_cmd) {

    case IM_USER_LOGIN: {
        /* 登录成功，body 是账号信息 */
        const im_acc_info_t* info = (const im_acc_info_t*)body;

        app->logged_in = 1;
        SAFE_STRNCPY(app->username,
            sizeof(app->username),
            info->username);

        printf("[LOGIN OK] user=%s\n", info->username);
        break;
    }

    case IM_USER_REGISTER:
        printf("[REGISTER OK]\n");
        break;

    case IM_USER_LOGOUT:
        app->logged_in = 0;
        SAFE_STRNCPY(app->username,
            sizeof(app->username),
            "unknown");
        printf("[LOGOUT OK] success\n");
        break;

    default:
        printf("[WARN] unknown account sub_cmd: %u\n",
            hdr->sub_cmd);
        break;
    }
}


