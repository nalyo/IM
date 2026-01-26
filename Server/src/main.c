#include "server_app.h"
#include "platform/im_thread.h"
#include <stdio.h>

int main(void)
{
    server_app_t app;
    if (server_app_start(&app, 8000) != IM_ERR_OK)
        return -1;

    printf("server running... waiting for clients\n");

    // ×èÈû main
    while (1) { im_thread_sleep_ms(1000); }

    server_app_stop(&app);
    return 0;
}
