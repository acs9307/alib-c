#ifndef CLIENT_LISTENER_PRIVATE_IS_DEFINED
#define CLIENT_LISTENER_PRIVATE_IS_DEFINED

#include "ClientListener.h"

/*******Private Structs*******/
struct epoll_pack
{
	int efd;
	struct epoll_event event;
	struct epoll_event triggered_events[DEFAULT_BACKLOG_SIZE];
};
/*****************************/

/* Object used for listening to a list of clients without having to open a new socket.  Basically
 * a server without using a port.  This is mainly used if the listening of sockets must be done on a
 * separate thread or in a separate instance from the server.  It is also useful if sockets are
 * being passed to your application from another application.
 *
 * NOTES:
 * 		When closing any related socket within a callback thread, DO NOT
 * 		call 'close()' unless you know what you are doing, ALWAYS use the
 * 		return value when possible.  If OUTSIDE a callback thread, use
 * 		the object's functions used for closing sockets whenever possible. */
struct ClientListener
{
	/* List of clients of type socket_package. */
	ArrayList* client_list;
	struct epoll_pack ep;

	/* Extended data. */
	void* ex_data;
	alib_free_value free_extended_data;

	/* Threading members. */
	pthread_t thread;
	flag_pole flag_pole;
	pthread_mutex_t mutex;
	pthread_cond_t t_cond;

	/* Callback members. */
	cl_client_data_ready_cb data_ready;
	cl_client_data_in_cb data_in;
	cl_client_disconnected_cb disconnected;
	cl_client_list_empty_cb client_list_empty;
};

#endif
