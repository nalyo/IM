#include "handlers.h"
#include <stdio.h>
#include <string.h>
#include "client_app.h"   // client_app_t
#include "utils/im_string.h"
#include "log.h"

void handle_user(client_app_t* app,
    const im_msg_hdr_t* hdr,
    const void* body)
{
    if (!app || !hdr)
        return;

    /* ---------------- 先处理错误 ---------------- */
    if ((hdr->flags & IM_FLAG_RESPONSE) &&
        hdr->err_code != IM_ERR_OK)
    {
        log_info("[USER ERROR] %s",
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

        log_info("[LOGIN OK] user=%s", info->username);
        break;
    }

    case IM_USER_REGISTER:
        log_info("[REGISTER OK]");
        break;

    case IM_USER_LOGOUT:
        app->logged_in = 0;
        SAFE_STRNCPY(app->username,
            sizeof(app->username),
            "unknown");
        log_info("[LOGOUT OK] success");
        break;

    default:
        log_error("unknown user sub_cmd: %u",
            hdr->sub_cmd);
        break;
    }
}


