#include "client_app.h"
#include "platform/im_thread.h"
#include "log.h"
#include <stdio.h>

static client_app_t g_app;

static void on_exit(void) {
    client_app_stop(&g_app);
    log_debug("Client exited (signal)");
}

int main(int argc, char** argv)
{
    if (argc != 2) { log_fatal("usage: %s <server_ip>", argv[0]); return -1; }

    im_register_exit_cb(on_exit);

    if (client_app_start(&g_app, argv[1], 8000) != IM_ERR_OK) {
        log_fatal("Failed to start client");
        return -1;
    }

    log_debug("Client running... type commands and hit enter");

    while (g_app.running) {
        im_thread_sleep_ms(200);
    }

    client_app_stop(&g_app);
    return 0;
}
