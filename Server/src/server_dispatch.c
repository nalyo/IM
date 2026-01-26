#include "server_dispatch.h"
#include "handlers/handlers.h"
#include <stdio.h>

void server_dispatch(client_info_t* client, const im_msg_hdr_t* hdr, const void* body)
{
    switch (hdr->main_cmd) {
    case IM_MAIN_USER:
        handle_user(client, hdr, body);
        break;
    case IM_MAIN_CHAT:
        handle_chat(client, hdr, body);
        break;
    case IM_MAIN_FRIEND:
        handle_friend(client, hdr, body);
        break;
    //case IM_MAIN_SYSTEM:
    //    handle_system(fd, sub_cmd, body);
        //break;
    default:
        printf("[WARN] unknown main_cmd: %u\n", hdr->main_cmd);
        break;
    }
}
