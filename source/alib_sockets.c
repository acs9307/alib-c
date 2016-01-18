#include "alib_sockets.h"

/* Sets the given socket to non-blocking mode.
 *
 * Returns:
 * 		alib_error
 * 		Return code from 'fcntl()'. */
int set_sock_non_block(int sock)
{
	int err;

	if(sock < 0)return(ALIB_BAD_ARG);

	err = fcntl(sock, F_SETFL, fcntl(sock, F_GETFL, 0) | O_NONBLOCK);
	if(err)
		return(err);
	else
		return(ALIB_OK);
}
