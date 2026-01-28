#include "client_plugin.h"
#include "client_app.h"
#include "im_protocol.h"
#include "log.h"

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include <curl/curl.h>
#include "cJSON.h"  // 需要把 cJSON.h/cJSON.c 放到项目里
#ifdef _WIN32
#define STRDUP _strdup
#else
#define STRDUP strdup
#endif

#define LLAMA_SERVER_IP "192.168.194.1"
#define LLAMA_SERVER_PORT 8080

// ------------------- curl 回调 -------------------
struct string {
    char* ptr;
    size_t len;
};

static void init_string(struct string* s) {
    s->len = 0;
    s->ptr = malloc(1);
    if (!s->ptr) exit(1);
    s->ptr[0] = '\0';
}

static size_t writefunc(void* ptr, size_t size, size_t nmemb, struct string* s) {
    size_t new_len = s->len + size * nmemb;
    s->ptr = realloc(s->ptr, new_len + 1);
    if (!s->ptr) exit(1);
    memcpy(s->ptr + s->len, ptr, size * nmemb);
    s->ptr[new_len] = '\0';
    s->len = new_len;
    return size * nmemb;
}

// ------------------- llama.cpp 调用 -------------------
static char* query_llama(const char* msg) {
    CURL* curl;
    CURLcode res;
    struct string s;
    init_string(&s);

    char url[256];
    snprintf(url, sizeof(url), "http://%s:%d/v1/chat/completions", LLAMA_SERVER_IP, LLAMA_SERVER_PORT);

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (!curl) return NULL;

    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    // 构造 JSON 请求
    char json_body[1024];
    snprintf(json_body, sizeof(json_body),
        "{"
        "\"model\":\"local\","
        "\"messages\":[{\"role\":\"user\",\"content\":\"%s\"}]"
        "}", msg);

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_body);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writefunc);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &s);

    res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    if (res != CURLE_OK) {
        log_error("[AutoReplyPlugin] curl failed: %s", curl_easy_strerror(res));
        free(s.ptr);
        return NULL;
    }

    // 解析 JSON
    cJSON* root = cJSON_Parse(s.ptr);
    free(s.ptr);
    if (!root) return NULL;

    cJSON* choices = cJSON_GetObjectItem(root, "choices");
    if (!choices || !cJSON_IsArray(choices)) {
        cJSON_Delete(root);
        return NULL;
    }

    cJSON* first = cJSON_GetArrayItem(choices, 0);
    if (!first) { cJSON_Delete(root); return NULL; }

    cJSON* message = cJSON_GetObjectItem(first, "message");
    if (!message) { cJSON_Delete(root); return NULL; }

    cJSON* content = cJSON_GetObjectItem(message, "content");
    if (!content || !cJSON_IsString(content)) { cJSON_Delete(root); return NULL; }

    char* result = STRDUP(content->valuestring); // Windows strdup
    cJSON_Delete(root);
    return result;
}

// ------------------- 插件核心 -------------------
static void auto_reply_on_message(client_app_t* app,
    const im_msg_hdr_t* hdr,
    const void* body) {

    if (!app || !hdr || !body) return;
    if (hdr->main_cmd != IM_MAIN_CHAT || hdr->sub_cmd != IM_CHAT_PUSH) return;

    const im_chat_msg_t* chat = (const im_chat_msg_t*)body;
    if (strcmp(chat->from, app->username) == 0) return;

    // 调用 llama.cpp
    char* reply_text = query_llama(chat->message);
    if (!reply_text) {
        log_warn("[AutoReplyPlugin] Llama query failed, fallback to 'hello'");
        reply_text = STRDUP("hello");
    }

    im_chat_msg_t resp = { 0 };
    SAFE_STRNCPY(resp.to, sizeof(resp.to), chat->from);
    SAFE_STRNCPY(resp.message, sizeof(resp.message), reply_text);
    resp.timestamp = (int)time(NULL);

    if (app->g_plugin_cb.send_message)
        app->g_plugin_cb.send_message(app->sock,
            IM_MAIN_CHAT,
            IM_CHAT_SEND,
            &resp,
            sizeof(resp));

    log_info("[AutoReplyPlugin] Sent '%s' to %s", resp.message, chat->from);

    free(reply_text);
}

// ------------------- DLL 导出 -------------------
#if defined(_WIN32) || defined(_WIN64)
__declspec(dllexport)
#endif
client_plugin_t* plugin_init() {
    static client_plugin_t plugin = {
        .name = "AutoReplyLlamaPlugin",
        .on_message = auto_reply_on_message,
        .on_input = NULL,
        .on_start = NULL,
        .on_stop = NULL,
        .enabled = 0,
        .next = NULL
    };
    return &plugin;
}
