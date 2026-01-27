#include "cmd_common.h"
#include "im_protocol.h"
#include <stdio.h>
static int cmd_plugin_load(client_app_t* app, const char* args)
{
    char target[IM_MAX_USERNAME] = { 0 };
    const char* p = args;

    if (read_token(&p, target, sizeof(target)) < 0) {
        printf("usage: plugin_load <plugin>\n");
        return -1;
    }

    client_plugin_t* plugin = app->g_plugin;
    while (plugin) {
        if (strcmp(plugin->name, target) == 0) {
            if (plugin->enabled) {
                printf("Plugin '%s' is already enabled.\n", plugin->name);
                return 0;
            }
            plugin->enabled = 1;
            if (plugin->on_start) plugin->on_start(app);
            printf("Plugin '%s' enabled.\n", plugin->name);
            return 0;
        }
        plugin = plugin->next;
    }
    printf("Plugin '%s' not found.\n", target);
    return -1;
}

static int cmd_plugin_unload(client_app_t* app, const char* args)
{

    char target[IM_MAX_USERNAME] = { 0 };
    const char* p = args;

    if (read_token(&p, target, sizeof(target)) < 0) {
        printf("usage: plugin_unload <plugin>\n");
        return -1;
    }

    client_plugin_t* plugin = app->g_plugin;
    while (plugin) {
        if (strcmp(plugin->name, target) == 0) {
            if (!plugin->enabled) {
                printf("Plugin '%s' is already disabled.\n", plugin->name);
                return 0;
            }
            if (plugin->on_stop) plugin->on_stop(app);
            plugin->enabled = 0;
            printf("Plugin '%s' disabled.\n", plugin->name);
            return 0;
        }
        plugin = plugin->next;
    }
    printf("Plugin '%s' not found.\n", target);
    return -1;
}

static int cmd_help(client_app_t* app, const char* args)
{
    (void)app; (void)args;
    printf(
        "Available commands:\n"
        "  User commands:\n"
        "    register <username> <password>   - Register a new account\n"
        "    login    <username> <password>   - Login with your account\n"
        "    logout                           - Logout from current account\n"
        "\n"
        "  Friend commands:\n"
        "    fadd <username>                  - Add a new friend\n"
        "    fdel <username>                  - Delete a friend\n"
        "    flist                            - Show friend list\n"
        "\n"
        "  Chat commands:\n"
        "    chat <username> <message>        - Send a message to a friend\n"
        "    history <username>               - Show chat history with a friend\n"
        "\n"
        "  System commands:\n"
        "    help                             - Show this help message\n"
        "    quit                             - Exit the application\n"
    );
    return 0;
}


static int cmd_quit(client_app_t* app, const char* args)
{
    (void)args;
    printf("Exiting...\n");
    client_app_stop(app);
    return 0;
}

const cmd_entry_t g_system_cmds[] = {
    { "plugin_load", cmd_plugin_load, IM_SYS_PLUGIN_LOAD },
    { "plugin_unload", cmd_plugin_unload, IM_SYS_PLUGIN_UNLOAD },
    { "help", cmd_help, IM_SYS_HELP },
    { "quit", cmd_quit, IM_SYS_QUIT },
};
const size_t g_system_cmds_count =
sizeof(g_system_cmds) / sizeof(g_system_cmds[0]);
