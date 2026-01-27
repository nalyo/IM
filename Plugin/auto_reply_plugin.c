#include "client_plugin.h"
#include "client_app.h"
#include "im_protocol.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

static void auto_reply_on_message(client_app_t* app,
    const im_msg_hdr_t* hdr,
    const void* body) {
    if (!app || !hdr || !body) return;
    if (hdr->main_cmd != IM_MAIN_CHAT || hdr->sub_cmd != IM_CHAT_PUSH) return;

    const im_chat_msg_t* chat = (const im_chat_msg_t*)body;
    if (strcmp(chat->from, app->username) == 0) return;


    im_chat_msg_t resp = { 0 };
    SAFE_STRNCPY(resp.to, sizeof(resp.to), chat->from);
    SAFE_STRNCPY(resp.message, sizeof(resp.message), "hello");
    resp.timestamp = (int)time(NULL);
    if (app->g_plugin_cb.send_message)
        app->g_plugin_cb.send_message(app->sock,
            IM_MAIN_CHAT,
            IM_CHAT_SEND,
            &resp,
            sizeof(resp));
    printf("\n[AutoReplyPlugin] Sent 'hello' to %s\n", chat->from);
}

#if defined(_WIN32) || defined(_WIN64)
__declspec(dllexport)
#endif
client_plugin_t* plugin_init() {
    static client_plugin_t plugin = {
        .name = "AutoReplyPlugin",
        .on_message = auto_reply_on_message,
        .on_input = NULL,
        .on_start = NULL,
        .on_stop = NULL,
        .enabled = 0,
        .next = NULL
    };
    return &plugin;
}
