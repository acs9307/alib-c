#ifndef ALIB_SOCKETS_IS_DEFINED
#define ALIB_SOCKETS_IS_DEFINED

#include <fcntl.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "alib_error.h"

/* Sets the given socket to non-blocking mode.
 *
 * Returns:
 * 		Return code from 'fcntl()'. */
#define set_sock_non_block(sock) \
	fcntl(sock, F_SETFL, fcntl(sock, F_GETFL, 0) | O_NONBLOCK)

/* Sets a timeout for the socket.
 *
 * Returns code from 'setsockopt()'.
 *
 * Does not check for socket validity. */
int set_sock_recv_timeout(int sock, size_t secs, size_t micros);

#endif
