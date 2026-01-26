#include "im_protocol.h"
#include "im_err.h"
#include "platform/im_socket.h"
#include <string.h>

int im_encode(uint16_t main_cmd,
    uint16_t sub_cmd,
    uint16_t flags,
    uint16_t err_code,
    const void* body,
    uint16_t len,
    void* out,
    uint16_t* out_len)
{
    if (!out || !out_len)
        return IM_ERR_INVALID_ARG;

    if (len > IM_MAX_PAYLOAD)
        return IM_ERR_PROTO_ENCODE;

    im_msg_hdr_t hdr;
    memset(&hdr, 0, sizeof(hdr));

    hdr.main_cmd = im_htons(main_cmd);
    hdr.sub_cmd = im_htons(sub_cmd);
    hdr.flags = im_htons(flags);
    hdr.err_code = im_htons(err_code);
    hdr.len = im_htons(len);

    memcpy(out, &hdr, sizeof(hdr));

    if (len > 0 && body)
        memcpy((uint8_t*)out + sizeof(hdr), body, len);

    *out_len = sizeof(hdr) + len;
    return IM_ERR_OK;
}


int im_decode(const void* buf,
    int buf_len,
    im_msg_hdr_t* hdr,
    const void** body)
{
    if (!buf || !hdr || !body)
        return IM_ERR_INVALID_ARG;

    if (buf_len < (int)sizeof(im_msg_hdr_t))
        return IM_ERR_PROTO_DECODE;

    memcpy(hdr, buf, sizeof(im_msg_hdr_t));

    hdr->main_cmd = im_ntohs(hdr->main_cmd);
    hdr->sub_cmd = im_ntohs(hdr->sub_cmd);
    hdr->flags = im_ntohs(hdr->flags);
    hdr->err_code = im_ntohs(hdr->err_code);
    hdr->len = im_ntohs(hdr->len);

    if (hdr->len > IM_MAX_PAYLOAD)
        return IM_ERR_PROTO_DECODE;

    if (buf_len < (int)(sizeof(im_msg_hdr_t) + hdr->len))
        return IM_ERR_PROTO_DECODE;

    *body = hdr->len
        ? (const uint8_t*)buf + sizeof(im_msg_hdr_t)
        : NULL;

    return IM_ERR_OK;
}

