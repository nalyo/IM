#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
    typedef SOCKET im_sock_t;
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
    typedef int im_sock_t;
#define INVALID_SOCKET (-1)
#endif
#define IM_SOCK_VALID(s) ((s) != INVALID_SOCKET)

    // ----------------------------
    // 网络初始化/清理
    // ----------------------------
    int im_net_init(void);
    void im_net_cleanup(void);

    // ----------------------------
    // Socket 创建/关闭
    // ----------------------------
    im_sock_t im_socket_create(void);
    int im_socket_close(im_sock_t s);

    // ----------------------------
    // Socket 发送/接收
    // ----------------------------
    int im_socket_send(im_sock_t s, const void* buf, int len);
    int im_socket_recv(im_sock_t s, void* buf, int len);

    // ----------------------------
    // Socket 大小端转换
    // ----------------------------
    uint16_t im_htons(uint16_t hostshort);
    uint16_t im_ntohs(uint16_t netshort);

#ifdef __cplusplus
}
#endif
