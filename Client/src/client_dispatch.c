#include "client_dispatch.h"
#include "handlers/handlers.h"
#include "client_plugin.h"
#include "log.h"
#include <stdio.h>

void client_dispatch(client_app_t* app, const im_msg_hdr_t* hdr, const void* body)
{
    client_plugin_dispatch_message(app->g_plugin, app, hdr, body);
    switch (hdr->main_cmd) {

    case IM_MAIN_USER:
        handle_user(app, hdr, body);
        break;

    case IM_MAIN_FRIEND:
        handle_friend(app, hdr, body);
        break;

    case IM_MAIN_CHAT:
        handle_chat(hdr, body);
        break;

    case IM_MAIN_SYSTEM:
        handle_system(hdr->sub_cmd, body);
        break;

    default:
        log_warn("unknown main_cmd: %u", hdr->main_cmd);
        break;
    }
}
