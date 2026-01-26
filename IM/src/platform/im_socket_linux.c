#ifndef _WIN32
#include "im_socket.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>


int im_net_init(void) { return 0; }
void im_net_cleanup(void) {}

im_sock_t im_socket_create(void)
{
    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        perror("socket");
        return -1;
    }
    return s;
}

int im_socket_close(im_sock_t s)
{
    return close(s);
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
