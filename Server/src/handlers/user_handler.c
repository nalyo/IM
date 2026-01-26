#include "handlers.h"
#include "send/send.h"
#include "managers/user_manager.h"
#include <stdio.h>
#include <string.h>

/* 账户处理 Handler */
void handle_user(client_info_t* client,
    const im_msg_hdr_t* hdr,
    const void* body)
{
    if (!client || !hdr)
        return;

    /* 只处理请求，不处理响应 */
    if (hdr->flags & IM_FLAG_RESPONSE)
        return;

    switch (hdr->sub_cmd) {

        /* ================= 注册 ================= */
    case IM_USER_REGISTER: {
        const im_auth_t* auth = (const im_auth_t*)body;

        im_err_t err = client_register(auth->username, auth->password);
        const char* strerror = im_strerror(err);
        printf("[USER] register: %s, result: %s \n", auth->username, strerror);

        server_send_packet(
            client->fd,
            IM_MAIN_USER,
            IM_USER_REGISTER,
            IM_FLAG_RESPONSE,
            err,
            NULL,
            0
        );
        break;
    }

                        /* ================= 登录 ================= */
    case IM_USER_LOGIN: {
        const im_auth_t* auth = (const im_auth_t*)body;

        im_err_t err = client_login(client, auth->username, auth->password);
        const char* strerror = im_strerror(err);
        printf("[USER] login: %s, result: %s \n", auth->username, strerror);

        if (err == IM_ERR_OK) {
            /* 登录成功，返回账户信息 */
            im_acc_info_t info;
            memset(&info, 0, sizeof(info));
            SAFE_STRNCPY(info.username, sizeof(info.username), client->username);

            server_send_packet(
                client->fd,
                IM_MAIN_USER,
                IM_USER_LOGIN,
                IM_FLAG_RESPONSE,
                IM_ERR_OK,
                &info,
                sizeof(info)
            );
        }
        else {
            server_send_packet(
                client->fd,
                IM_MAIN_USER,
                IM_USER_LOGIN,
                IM_FLAG_RESPONSE,
                err,
                NULL,
                0
            );
        }
        break;
    }

                     /* ================= 登出 ================= */
    case IM_USER_LOGOUT: {
        if (client && client->logged_in) {
            im_err_t err = client_logout(client);

            const char* strerror = im_strerror(err);
            printf("[USER] logout: %s, result: %s \n", client->username, strerror);
            server_send_packet(
                client->fd,
                IM_MAIN_USER,
                IM_USER_LOGOUT,
                IM_FLAG_RESPONSE,
                err,
                NULL,
                0
            );
        }
        break;
    }

    default:
        printf("[USER] unknown account sub_cmd: %u\n", hdr->sub_cmd);
        break;
    }
}
