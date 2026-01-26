#include "handlers.h"
#include <stdio.h>

void handle_system(uint16_t sub_cmd, const void* body)
{
    switch (sub_cmd) {

    case IM_NET_OK: {
        const im_ok_t* ok = (const im_ok_t*)body;
        printf("[OK] %s\n", ok->resp_msg);
        break;
    }

    case IM_NET_ERROR: {
        const im_error_t* err = (const im_error_t*)body;
        printf("[ERROR %d] %s\n", err->err_code, err->err_msg);
        break;
    }

    case IM_NET_KICK:
        printf("[SYSTEM] you have been kicked\n");
        break;

    default:
        printf("[WARN] unknown system sub_cmd: %u\n", sub_cmd);
        break;
    }
}
