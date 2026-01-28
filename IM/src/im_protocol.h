#pragma once
#include <stdint.h>
#include "utils/im_string.h"
#include "im_msg_type.h"

#ifdef __cplusplus
extern "C" {
#endif

    /* ============================================================
     * 1. 常量定义
     * ============================================================ */
#define IM_MAX_PAYLOAD      4096
#define IM_MAX_USERNAME     32
#define IM_MAX_PASSWORD     32
#define IM_MAX_FRIEND_NAME  32
#define IM_MAX_MESSAGE      256
#define IM_MAX_ERR_MSG      64

#define IM_FLAG_REQUEST  0x00
#define IM_FLAG_RESPONSE  0x01

#define IM_TIME_LEN  32

    /* ============================================================
     * 3. 通用消息头
     * ============================================================ */
#pragma pack(push, 1)
    typedef struct {
        uint16_t main_cmd;
        uint16_t sub_cmd;

        uint16_t flags;
        uint16_t  err_code;

        uint16_t len;
    } im_msg_hdr_t;
#pragma pack(pop)



    /* ============================================================
     * 4. Payload 结构定义
     * ============================================================ */
#pragma pack(push, 1)

     /* ---------- 注册 / 登录 ---------- */
    typedef struct {
        char username[IM_MAX_USERNAME];
        char password[IM_MAX_PASSWORD];
    } im_auth_t;

    typedef struct {
        char username[IM_MAX_USERNAME];
    } im_acc_info_t;

    /* ---------- 添加好友 ---------- */
    typedef struct {
        char username[IM_MAX_FRIEND_NAME];
    } im_friend_info_t;

    /* 好友列表结构 */
    typedef struct {
        uint16_t count;
        struct {
            char username[IM_MAX_FRIEND_NAME];
            uint8_t online;  // 0=离线, 1=在线
        } friends[0]; // 可变数组，根据 count 计算长度
    } im_friend_list_t;

    /* ---------- 成功响应 ---------- */
    typedef struct {
        char    resp_info[IM_MAX_MESSAGE];
        char    resp_msg[IM_MAX_MESSAGE];
    } im_ok_t;

    /* ---------- 聊天（请求 & 推送共用） ---------- */
    typedef struct chat_msg_t {
        char from[IM_MAX_USERNAME];
        char to[IM_MAX_USERNAME];
        char message[IM_MAX_MESSAGE];
        int64_t  timestamp;   // 可选，Unix 时间戳
    } im_chat_msg_t;

    /* ---------- 错误响应 ---------- */
    typedef struct {
        int32_t err_code;
        char    err_msg[IM_MAX_ERR_MSG];
    } im_error_t;

#pragma pack(pop)

    /* ============================================================
     * 6. 编码 / 解码接口
     * ============================================================ */

     /**
      * 编码：cmd + body -> buffer
      * @param cmd       命令类型
      * @param body      payload 数据指针（可为 NULL）
      * @param body_len  payload 长度
      * @param out       输出 buffer（必须 >= sizeof(im_msg_hdr_t) + body_len）
      * @param out_len   输出总长度（header + payload）
      * @return 0 成功，<0 错误
      */
    int im_encode(uint16_t main_cmd,
        uint16_t sub_cmd,
        uint16_t flags,
        uint16_t err_code,
        const void* body,
        uint16_t len,
        void* out,
        uint16_t* out_len);


    /**
     * 解码：buffer -> header + body
     * @param buf       输入 buffer
     * @param buf_len   buffer 长度
     * @param hdr       输出消息头
     * @param body      输出 payload 指针（指向 buf 内部，不拷贝）
     * @return 0 成功，<0 错误（数据不完整或非法）
     */
    int im_decode(const void* buf,
        int buf_len,
        im_msg_hdr_t* hdr,
        const void** body);

#ifdef __cplusplus
}
#endif
