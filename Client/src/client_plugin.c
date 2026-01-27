#include "client_plugin.h"

void client_plugin_register(client_plugin_t** head, client_plugin_t* plugin) {
    if (!plugin) return;
    plugin->next = *head;
    *head = plugin;

    // 不传 NULL，插件 on_start 也要接收回调
    // 这里先初始化回调为空，客户端启动时可传入真正回调
    if (plugin->on_start) plugin->on_start(NULL);
}
// 启用插件
void client_plugin_enable(client_plugin_t* plugin) {
    if (plugin) plugin->enabled = 1;
}

// 禁用插件
void client_plugin_disable(client_plugin_t* plugin) {
    if (plugin) plugin->enabled = 0;
}

// ------------------------------
// 消息分发，增加回调参数
// ------------------------------
void client_plugin_dispatch_message(client_plugin_t* head, client_app_t* app, const im_msg_hdr_t* hdr, const void* body) {
    client_plugin_t* p = head;
    while (p) {
        if (p->enabled && p->on_message)
            p->on_message(app, hdr, body);
        p = p->next;
    }
}

// ------------------------------
// 输入分发，增加回调参数
// ------------------------------
void client_plugin_dispatch_input(client_plugin_t* head, client_app_t* app, const char* input) {
    client_plugin_t* p = head;
    while (p) {
        if (p->enabled && p->on_input)
            p->on_input(app, input);
        p = p->next;
    }
}
