#ifndef TCP_SERVER_PRIVATE_IS_DEFINED
#define TCP_SERVER_PRIVATE_IS_DEFINED

#include "TcpServer.h"

/* Simple TcpServer object used to handle incoming TCP connections.
 * Listening can be done either on a single thread or on a second thread.
 *
 * All callbacks run on the same thread. */
struct TcpServer
{
	/* The socket of the server, set to -1 when not in use. */
	int sock;
	/* The address struct of the server. */
	struct sockaddr_in addr;

	/* Listening thread. */
	pthread_t event_thread;
	flag_pole flag_pole;

	/* List of clients. List type is of 'socket_package'. */
	ArrayList* client_list;

	/* Called whenever a client connects to the server. */
	ts_client_connected_cb client_connected;
	/* Called whenever data is ready on the client socket.
	 * This should only be set if 'recv()' should not be used. */
	ts_client_data_ready_cb client_data_ready;
	/* Called whenever data is received from a client. */
	ts_client_data_in_cb client_data_in;
	/* Called whenever a client disconnects from the server. */
	ts_client_disconnected_cb client_disconnected;
	/* Called whenever the listening thread is about to return. */
	ts_thread_returning_cb thread_returning;

	/* Extended data for the server. */
	void* ex_data;
	/* Callback used to free the server. */
	alib_free_value free_data_cb;
};


#endif
