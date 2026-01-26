#pragma once
#include <stdint.h>
#include "platform/im_socket.h"   // 统一 im_sock_t 定义

#ifdef __cplusplus
extern "C" {
#endif

	/**
	 * 创建监听 socket
	 * port    : 监听端口
	 * backlog : listen backlog
	 * 返回   : socket fd / INVALID_SOCKET (-1 Linux)
	 */
	im_sock_t server_listen(uint16_t port, int backlog);

	/**
	 * accept 新连接
	 * 返回   : 客户端 socket fd / INVALID_SOCKET (-1 Linux)
	 */
	im_sock_t server_accept(im_sock_t listen_fd);

#ifdef __cplusplus
}
#endif
