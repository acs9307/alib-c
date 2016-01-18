#ifndef FD_SERVER_PRIVATE_IS_DEFINED
#define FD_SERVER_PRIVATE_IS_DEFINED

#include "FdServer.h"
#include "FdClient_private.h"

/*******Structs*******/
typedef struct epoll_package
{
	int efd;
	struct epoll_event event;
	struct epoll_event events[DEFAULT_BACKLOG_SIZE];
}epoll_package;
/*********************/

/*******Classes*******/
/* A simple server object that handles incoming sockets that are
 * AF_UNIX family.  Solely designed to handle incoming file descriptor
 * messages.  Will only read 1 file descriptor per message.  If multiple
 * file descriptors must be sent, they must be sent separately.
 *
 * Look at FdClient for a simple method of connecting and sending
 * file descriptors to the server.
 */
struct FdServer
{
	int sock;
	struct sockaddr_un addr;

	/* User defined callbacks. */
	fscb_on_connect on_connect;
	fscb_on_receive on_receive;
	fscb_on_disconnect on_disconnect;

	/* List of client sockets.
	 * This should never be null. */
	ArrayList* clients;

	/* Read thread members. */
	pthread_t thread;
	flag_pole flag_pole;

	void* extended_data;
	alib_free_value free_extended_data;

	epoll_package ep;
};
/*********************/

/* Function Prototypes */
static alib_error run_loop(FdServer* server);
/***********************/

#endif
