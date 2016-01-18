#ifndef FD_CLIENT_PRIVATE_IS_DEFINED
#define FD_CLIENT_PRIVATE_IS_DEFINED

#include "FdClient.h"

/*******Classes*******/
/* Simple object that sends file descriptors using AF_UNIX
 * protocol.  It is specifically designed to work with FdServer. */
struct FdClient
{
	/* Socket to the connected host.  If not connected,
	 * sock will be -1. */
	int sock;
	/* Address of the host. */
	struct sockaddr_un addr;

	pthread_t thread;
	flag_pole flag_pole;

	void* extended_data;
	alib_free_value free_extended_data;
};
/*********************/

#endif
