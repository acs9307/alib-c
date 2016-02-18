#ifndef TCP_CLIENT_PRIVATE_IS_DEFINED
#define TCP_CLIENT_PRIVATE_IS_DEFINED

#include "TcpClient.h"

/*******Class Definition*******/
/* Object used to transmit data over a TCP socket.
 *
 * Supports sending and receiving of data.
 *
 * If the receiver is setup, it will run on a separate thread. */
struct TcpClient
{
	int sock;
	struct sockaddr_in host_addr;

	/* Callback Members */
	tc_data_in data_in_cb;
	tc_disconnect disconnect_cb;
	tc_sockopt sockopt_cb;
	tc_thread_returning thread_returning_cb;

	/* Transmission Members. */
	/* This is the thread that handles reading of
	 * data. */
	pthread_t read_thread;
	flag_pole flag_pole;

	void* ex_data;
	alib_free_value free_data_cb;
};
/*******************************/

#endif
