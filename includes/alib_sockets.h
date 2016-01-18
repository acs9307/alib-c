#ifndef ALIB_SOCKETS_IS_DEFINED
#define ALIB_SOCKETS_IS_DEFINED

#include <fcntl.h>

#include "alib_error.h"

/* Sets the given socket to non-blocking mode.
 *
 * Returns:
 * 		alib_error
 * 		Return code from 'fcntl()'. */
int set_sock_non_block(int sock);

#endif
