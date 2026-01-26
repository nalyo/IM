#include "send.h"
#include "platform/im_socket.h"
#include "im_err.h"

#include <stdio.h>

void server_send_packet(im_sock_t fd,
    uint16_t main_cmd,
    uint16_t sub_cmd,
    uint16_t flag,
    uint16_t err_code,
    const void* body,
    uint16_t body_len)
{
    char buf[sizeof(im_msg_hdr_t)+IM_MAX_PAYLOAD];
    uint16_t out_len;

    if (im_encode(
        main_cmd,
        sub_cmd,
        flag,
        err_code,
        body,
        body_len,
        buf,
        &out_len) == IM_ERR_OK)
    {
        if (im_socket_send(fd, buf, out_len) < 0)
        {
#ifdef WIN32
            int err = WSAGetLastError();
            printf("send failed: %d\n", err);
#else
            perror("send");
#endif
        }
    }
}

