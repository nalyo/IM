#include "cmd_common.h"
#include "utils/parse.h"
#include "send/send.h"
#include <stdio.h>
#include "im_protocol.h"

#include "cmd_common.h"
#include "utils/parse.h"
#include "send/send.h"
#include <stdio.h>
#include "im_protocol.h"

static int cmd_register(client_app_t* app, const char* args)
{
    if (app->logged_in) {
        printf("already logged in as %s, please logout first\n", app->username);
        return 0;
    }

    char user[IM_MAX_USERNAME], pass[IM_MAX_PASSWORD];
    const char* p = args;

    if (read_token(&p, user, sizeof(user)) < 0 ||
        read_token(&p, pass, sizeof(pass)) < 0) {
        printf("usage: register <user> <pass>\n");
        return -1;
    }

    im_auth_t auth = { 0 };
    SAFE_STRNCPY(auth.username, sizeof(auth.username), user);
    SAFE_STRNCPY(auth.password, sizeof(auth.password), pass);

    return client_send_packet(app->sock,
        IM_MAIN_USER,
        IM_USER_REGISTER,
        &auth,
        sizeof(auth));
}

static int cmd_login(client_app_t* app, const char* args)
{
    if (app->logged_in) {
        printf("already logged in as %s\n", app->username);
        return 0;
    }

    char user[IM_MAX_USERNAME], pass[IM_MAX_PASSWORD];
    const char* p = args;

    if (read_token(&p, user, sizeof(user)) < 0 ||
        read_token(&p, pass, sizeof(pass)) < 0) {
        printf("usage: login <user> <pass>\n");
        return -1;
    }

    im_auth_t auth = { 0 };
    SAFE_STRNCPY(auth.username, sizeof(auth.username), user);
    SAFE_STRNCPY(auth.password, sizeof(auth.password), pass);

    return client_send_packet(app->sock,
        IM_MAIN_USER,
        IM_USER_LOGIN,
        &auth,
        sizeof(auth));
}

/* ====================== 注销 ====================== */
static int cmd_logout(client_app_t* app, const char* args)
{
    (void)args; // 忽略参数

    if (!app->logged_in) {
        printf("not logged in\n");
        return 0;
    }

    // 发送注销请求，body 可以为空
    int ret = client_send_packet(app->sock,
        IM_MAIN_USER,
        IM_USER_LOGOUT,
        NULL,
        0);

    return ret;
}

/* 导出表 */
const cmd_entry_t g_account_cmds[] = {
    { "register", cmd_register, IM_USER_REGISTER },
    { "login",    cmd_login, IM_USER_LOGIN },
    { "logout",   cmd_logout,   IM_USER_LOGOUT },
};
const size_t g_account_cmds_count =
sizeof(g_account_cmds) / sizeof(g_account_cmds[0]);
