#include "alib_sockets.h"

/* Sets a timeout for the socket.
 *
 * Returns code from 'setsockopt()'.
 *
 * Does not check for socket validity. */
int set_sock_recv_timeout(int sock, size_t secs, size_t micros)
{
	struct timeval tv;
	tv.tv_sec = secs;
	tv.tv_usec = micros;
	return(setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)));
}
