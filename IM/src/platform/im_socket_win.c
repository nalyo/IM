#ifdef _WIN32
#include "im_socket.h"
#include "log.h"
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

int im_net_init(void)
{
    WSADATA wsa;
    int ret = WSAStartup(MAKEWORD(2, 2), &wsa);
    if (ret != 0) {
        log_fatal("WSAStartup failed: %d", ret);
        return -1;
    }
    return 0;
}

void im_net_cleanup(void)
{
    WSACleanup();
}

im_sock_t im_socket_create(void)
{
    im_sock_t s = socket(AF_INET, SOCK_STREAM, 0);
    if (s == INVALID_SOCKET) return INVALID_SOCKET;
    return s;
}

int im_socket_close(im_sock_t s)
{
    return closesocket(s);
}

int im_socket_send(im_sock_t s, const void* buf, int len)
{
    return send(s, buf, len, 0);
}

int im_socket_recv(im_sock_t s, void* buf, int len)
{
    return recv(s, buf, len, 0);
}

uint16_t im_htons(uint16_t hostshort)
{
    return htons(hostshort);
}

uint16_t im_ntohs(uint16_t netshort)
{
    return ntohs(netshort);
}

#endif