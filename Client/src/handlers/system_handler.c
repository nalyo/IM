#include "handlers.h"
#include "log.h"
#include <stdio.h>

void handle_system(uint16_t sub_cmd, const void* body)
{
    switch (sub_cmd) {

    case IM_NET_OK: {
        const im_ok_t* ok = (const im_ok_t*)body;
        log_info("[OK] %s", ok->resp_msg);
        break;
    }

    case IM_NET_ERROR: {
        const im_error_t* err = (const im_error_t*)body;
        log_info("[ERROR %d] %s", err->err_code, err->err_msg);
        break;
    }

    case IM_NET_KICK:
        log_info("[SYSTEM] you have been kicked");
        break;

    default:
        log_warn("unknown system sub_cmd: %u", sub_cmd);
        break;
    }
}
