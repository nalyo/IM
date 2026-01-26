#include "cmd_common.h"
#include "im_protocol.h"
#include <stdio.h>

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
    { "help", cmd_help, IM_SYS_HELP },
    { "quit", cmd_quit, IM_SYS_QUIT },
};
const size_t g_system_cmds_count =
sizeof(g_system_cmds) / sizeof(g_system_cmds[0]);
