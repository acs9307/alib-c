#ifndef FD_CLIENT_IS_DEFINED
#define FD_CLIENT_IS_DEFINED

#include <sys/un.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <inttypes.h>
#include <stdlib.h>
#include <pthread.h>

#include "ancillary.h"
#include "alib_error.h"
#include "alib_types.h"
#include "server_defines.h"
#include "flags.h"

/*******Classes*******/
/* Simple object that sends file descriptors using AF_UNIX
 * protocol.  It is specifically designed to work with FdServer. */
typedef struct FdClient FdClient;
/*********************/

/*******Function Pointer Types*******/
/* Called whenever a file descriptor was received
 * on the client's socket.
 *
 * Parameters:
 * 		client: The FdClient that received the FD.
 * 		fd: The file descriptor received.
 */
typedef void(*fdc_fd_received_cb)(FdClient*, int);
/* Called whenever the client is disconnected from the server
 * whether or not it be intentional. */
typedef void(*fdc_fd_disconnect_cb)(FdClient*);
/************************************/

/*******Public Functions*******/
/* Closes an FdClient by disconnecting it from the host. */
void FdClient_close(FdClient* sender);

/* Connects the sender to the host. */
alib_error FdClient_connect(FdClient* sender);

/* Sends a file descriptor to the host. */
alib_error FdClient_send(FdClient* sender, int fd);

/* Starts the FdClient listening for incoming data on the object's socket.
 *
 * Parameters:
 * 		client: (REQUIRED) The FdClient object to start listening.
 * 		fd_received: (REQUIRED) Called when data is received on the client's socket.
 * 		fd_disconnected: (OPTIONAL) Called whenever the client's socket is closed, either
 * 			on the host's side or the client's side.
 *
 * Returns:
 * 		ALIB_ERROR: Error that occurred during setup.
 * 		Anything else: Error code returned from pthread_create().
 * 		0: Success. */
alib_error FdClient_listen(FdClient* client, fdc_fd_received_cb fd_received);
/* Starts the FdClient listening for incoming data on the object's socket.
 * Listening will be run on a separate thread managed internally.
 *
 * Parameters:
 * 		client: (REQUIRED) The FdClient object to start listening.
 * 		fd_received: (REQUIRED) Called when data is received on the client's socket.
 * 		fd_disconnected: (OPTIONAL) Called whenever the client's socket is closed, either
 * 			on the host's side or the client's side.
 *
 * Returns:
 * 		ALIB_ERROR: Error that occurred during setup.
 * 		Anything else: Error code returned from pthread_create().
 * 		0: Success. */
int FdClient_listen_async(FdClient* client, fdc_fd_received_cb fd_received,
		fdc_fd_disconnect_cb fd_disconnected);

	/* Getters */
/* Returns the socket of the sender.
 *
 * Assumes 'sender' is not null. */
int FdClient_get_sock(FdClient* sender);
/* Returns the sockaddr struct for the FdClient.
 *
 * Assumes 'sender' is not null. */
const struct sockaddr_un* FdClient_get_addr(FdClient* sender);

/* Returns the extended data member of the client.
 *
 * Assumes 'client' is not null. */
void* FdClient_get_extended_data(FdClient* client);
/* Returns 0 if the client is not connected, otherwise returned !0.
 *
 * Assumes 'client' is not null. */
char FdClient_is_connected(FdClient* client);
	/***********/

	/* Setters */
void FdClient_set_extended_data(FdClient* client, void* data, alib_free_value free_data,
		char free_old_data);
	/***********/
/******************************/

/*******Constructors*******/
/* Allocates a new FdClient according to the given file name.
 *
 * Parameters:
 * 		fname: This is the unique file name that the receiver will
 * 			be reading from.  This is used instead of a port due to
 * 			the fact that we have to use AF_UNIX protocols.
 * 		name_len: This is the length of the given 'fname'.  If
 * 			the value is 0, length of 'fname' will be calculated
 * 			by calling 'strlen(fname)'.  If the length is larger than
 * 			the length of (struct sockaddr_un).sun_path, then the name
 * 			will be truncated to fit into the buffer.
 *
 * Returns:
 * 		NULL: Error.
 * 		FdClient*: Newly allocated FdClient object. */
FdClient* newFdClient(const char* fname, size_t name_len);

/* Frees an FdClient object and sets the pointer to null. */
void delFdClient(FdClient** sender);
/**************************/

#endif
