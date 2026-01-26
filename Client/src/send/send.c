#include "send.h"
#include "im_protocol.h"
#include "platform/im_socket.h"
#include "im_err.h"

int client_send_packet(im_sock_t fd,
    uint16_t main_cmd,
    uint16_t sub_cmd,
    const void* body,
    uint16_t body_len)
{
    char buf[sizeof(im_msg_hdr_t) + IM_MAX_PAYLOAD];
    uint16_t out_len;

    if (im_encode(
        main_cmd,
        sub_cmd,
        IM_FLAG_REQUEST,
        IM_ERR_OK,
        body,
        body_len,
        buf,
        &out_len) == IM_ERR_OK)
    {
        return im_socket_send(fd, buf, out_len) >= 0 ? IM_ERR_OK : IM_ERR_PROTO_ENCODE;
    }
    return IM_ERR_PROTO_ENCODE;
}
