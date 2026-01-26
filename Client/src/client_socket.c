#include "client_socket.h"
#include <stdio.h>
#include <string.h>

im_sock_t client_connect(const char* ip, uint16_t port)
{
    im_sock_t s = im_socket_create();
    if (s == INVALID_SOCKET) return INVALID_SOCKET;

    struct sockaddr_in serv;
    memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &serv.sin_addr) <= 0)
    {
        printf("client_connect: invalid ip %s\n", ip);
        im_socket_close(s);
        return INVALID_SOCKET;
    }

    if (connect(s, (struct sockaddr*)&serv, sizeof(serv)) < 0)
    {
        perror("client_connect: connect failed");
        im_socket_close(s);
        return INVALID_SOCKET;
    }

    return s;
}
