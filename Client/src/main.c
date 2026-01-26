#include "client_app.h"
#include "platform/im_thread.h"
#include <stdio.h>

static client_app_t g_app;

static void on_exit(void) {
    client_app_stop(&g_app);
    printf("Client exited (signal)\n");
}

int main(int argc, char** argv)
{
    if (argc != 2) { printf("usage: %s <server_ip>\n", argv[0]); return -1; }

    im_register_exit_cb(on_exit);

    if (client_app_start(&g_app, argv[1], 8000) != IM_ERR_OK) {
        printf("Failed to start client\n");
        return -1;
    }

    printf("Client running... type commands and hit enter\n");

    while (g_app.running) {
        im_thread_sleep_ms(200);
    }

    client_app_stop(&g_app);
    return 0;
}
