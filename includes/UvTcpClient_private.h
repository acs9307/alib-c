#ifndef UV_TCP_CLIENT_PRIVATE_IS_DEFINED
#define UV_TCP_CLIENT_PRIVATE_IS_DEFINED

#include "UvTcpClient.h"

/*******Class Declarations*******/
struct UvTcpClient
{
	uv_tcp_t handle;
	uv_connect_t con_handle;
	int sock;
	struct sockaddr_in addr;

	/* Status Members */
	char connected;

	/* Callbacks */
	uc_client_connect_cb client_connect;
	uc_client_close_cb client_close;
	uc_client_read_cb client_read;

	/* The buffer used for receiving.  This is allocated once
	 * and only freed if the user calls for it to be freed or
	 * the client disconnects. */
	char* recv_buff;
	/* The number of bytes allocated by the recv buffer. */
	size_t recv_buff_size;

	/* Threading members. */
	pthread_t thread;
	char thread_running;
};
/********************************/

#endif
