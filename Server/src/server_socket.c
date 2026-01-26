#include "server_socket.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

im_sock_t server_listen(uint16_t port, int backlog)
{
    im_sock_t s = im_socket_create();
#ifdef _WIN32
    if (s == INVALID_SOCKET) return INVALID_SOCKET;
#else
    if (s < 0) return -1;
#endif

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    int opt = 1;
#ifdef _WIN32
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));
#else
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#endif

    if (bind(s, (struct sockaddr*)&addr, sizeof(addr)) < 0)
    {
        perror("server_listen: bind failed");
        im_socket_close(s);
#ifdef _WIN32
        return INVALID_SOCKET;
#else
        return -1;
#endif
    }

    if (listen(s, backlog) < 0)
    {
        perror("server_listen: listen failed");
        im_socket_close(s);
#ifdef _WIN32
        return INVALID_SOCKET;
#else
        return -1;
#endif
    }

    return s;
}

im_sock_t server_accept(im_sock_t listen_fd)
{
    struct sockaddr_in cli_addr;
#ifdef _WIN32
    int addrlen = sizeof(cli_addr);
#else
    socklen_t addrlen = sizeof(cli_addr);
#endif

    im_sock_t cfd = accept(listen_fd, (struct sockaddr*)&cli_addr, &addrlen);
#ifdef _WIN32
    if (cfd == INVALID_SOCKET) return INVALID_SOCKET;
#else
    if (cfd < 0) return -1;
#endif
    return cfd;
}
