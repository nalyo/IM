#include "client_input.h"
#include "utils/parse.h"
#include "client_plugin.h"
#include "log.h"

#include <string.h>
#include <stdio.h>

/* 声明各模块导出的命令表 */
extern const cmd_entry_t g_account_cmds[];
extern const size_t g_account_cmds_count;

extern const cmd_entry_t g_chat_cmds[];
extern const size_t g_chat_cmds_count;

extern const cmd_entry_t g_friend_cmds[];
extern const size_t g_friend_cmds_count;

extern const cmd_entry_t g_system_cmds[];
extern const size_t g_system_cmds_count;

static int dispatch_cmd(client_app_t* app,
    const char* cmd,
    const char* args,
    const cmd_entry_t* table,
    size_t count)
{
    for (size_t i = 0; i < count; ++i) {
        if (strcmp(cmd, table[i].name) == 0) {
            app->last_cmd = table[i].sub_cmd;
            return table[i].handler(app, args);
        }
    }
    return -1;
}

void client_handle_input(client_app_t* app, const char* line)
{
    char cmd[32];
    const char* p = line;

    if (read_token(&p, cmd, sizeof(cmd)) < 0)
        return;

    client_plugin_dispatch_input(app->g_plugin, app, line);
    if (dispatch_cmd(app, cmd, p, g_account_cmds, g_account_cmds_count) == 0) return;
    if (dispatch_cmd(app, cmd, p, g_chat_cmds, g_chat_cmds_count) == 0) return;
    if (dispatch_cmd(app, cmd, p, g_friend_cmds, g_friend_cmds_count) == 0) return;
    if (dispatch_cmd(app, cmd, p, g_system_cmds, g_system_cmds_count) == 0) return;

    log_warn("unknown command: %s", cmd);
}
