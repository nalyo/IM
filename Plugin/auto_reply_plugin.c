#include "client_plugin.h"
#include "client_app.h"
#include "im_protocol.h"
#include "log.h"

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include <curl/curl.h>
#include "cJSON.h"
#ifdef _WIN32
#define STRDUP _strdup
#else
#define STRDUP strdup
#endif

#define LLAMA_SERVER_IP "192.168.194.1"
#define LLAMA_SERVER_PORT 8080

#define MAX_HISTORY 10   // 保存最近 10 条消息

// ------------------- 消息历史存储 -------------------
typedef struct {
    char* role;   // "user" 或 "assistant"
    char* content;
} chat_msg_t;

typedef struct {
    chat_msg_t msgs[MAX_HISTORY];
    int count;
} chat_history_t;

static chat_history_t g_history = { 0 };

// 添加消息到历史
static void add_history(const char* role, const char* content) {
    if (g_history.count == MAX_HISTORY) {
        free(g_history.msgs[0].role);
        free(g_history.msgs[0].content);
        for (int i = 1; i < MAX_HISTORY; i++)
            g_history.msgs[i - 1] = g_history.msgs[i];
        g_history.count--;
    }
    g_history.msgs[g_history.count].role = STRDUP(role);
    g_history.msgs[g_history.count].content = STRDUP(content);
    g_history.count++;
}

// ------------------- JSON 转义 -------------------
static char* json_escape(const char* str) {
    size_t len = strlen(str);
    char* out = malloc(len * 6 + 1); // 最坏情况每个字符都要转义
    if (!out) return NULL;
    char* p = out;
    for (size_t i = 0; i < len; i++) {
        switch (str[i]) {
        case '\"': *p++ = '\\'; *p++ = '\"'; break;
        case '\\': *p++ = '\\'; *p++ = '\\'; break;
        case '\n': *p++ = '\\'; *p++ = 'n';  break;
        case '\r': *p++ = '\\'; *p++ = 'r';  break;
        case '\t': *p++ = '\\'; *p++ = 't';  break;
        default: *p++ = str[i]; break;
        }
    }
    *p = '\0';
    return out;
}

// 构建 messages JSON
static char* build_messages_json() {
    char buffer[4096] = { 0 };
    strcat(buffer, "[");
    for (int i = 0; i < g_history.count; i++) {
        char* escaped_content = json_escape(g_history.msgs[i].content);
        char tmp[512];
        snprintf(tmp, sizeof(tmp),
            "{\"role\":\"%s\",\"content\":\"%s\"}%s",
            g_history.msgs[i].role,
            escaped_content,
            (i == g_history.count - 1) ? "" : ",");
        free(escaped_content);
        strcat(buffer, tmp);
    }
    strcat(buffer, "]");
    return STRDUP(buffer);  // 调用后需 free
}

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
static char* query_llama_with_json(const char* json_body) {
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
    if (!choices || !cJSON_IsArray(choices)) { cJSON_Delete(root); return NULL; }

    cJSON* first = cJSON_GetArrayItem(choices, 0);
    if (!first) { cJSON_Delete(root); return NULL; }

    cJSON* message = cJSON_GetObjectItem(first, "message");
    if (!message) { cJSON_Delete(root); return NULL; }

    cJSON* content = cJSON_GetObjectItem(message, "content");
    if (!content || !cJSON_IsString(content)) { cJSON_Delete(root); return NULL; }

    char* result = STRDUP(content->valuestring);
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

    // 保存用户消息到历史
    add_history("user", chat->message);

    // 构建 JSON
    char* msgs_json = build_messages_json();
    char json_body[4096];
    snprintf(json_body, sizeof(json_body),
        "{\"model\":\"local\",\"messages\":%s, \"max_tokens\": 20}", msgs_json);
    free(msgs_json);

    log_debug(json_body);

    // 调用 llama
    char* reply_text = query_llama_with_json(json_body);
    if (!reply_text) {
        log_warn("[AutoReplyPlugin] Llama query failed, fallback to 'hello'");
        reply_text = STRDUP("hello");
    }

    // 保存助手回复到历史
    add_history("assistant", reply_text);

    // 发送消息
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
