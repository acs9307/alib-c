#ifndef FD_SERVER_IS_DEFINED
#define FD_SERVER_IS_DEFINED

#include <sys/un.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/epoll.h>

#include "server_defines.h"
#include "UvTcpServer_private.h"
#include "ArrayList.h"
#include "ancillary.h"
#include "flags.h"
#include "alib_sockets.h"
#include "alib_cb_funcs.h"

/*******Classes*******/
/* Simple package struct that will be attached to each client that connects.
 * This data will, in turn, be passed to the user's callbacks.  This should
 * never be freed by the user, memory and file descriptors will be handled internally. */
typedef struct fds_package
{
	const int sock;
	void* user_data;
	alib_free_value free_user_data;
}fds_package;

/* A simple server object that handles incoming sockets that are
 * AF_UNIX family.  Solely designed to handle incoming file descriptor
 * messages.  Will only read 1 file descriptor per message.  If multiple
 * file descriptors must be sent, they must be sent separately.
 *
 * Look at FdClient for a simple method of connecting and sending
 * file descriptors to the server. */
typedef struct FdServer FdServer;
/*********************/

/*******Callback Function Types*******/
/* Called whenever a client connects to the FdServer.
 *
 * Parameters:
 * 		server: The server that accepted the new client.
 * 		client_package: The package containing meta data about the
 * 			client.
 *
 * Return Behavior:
 * 		SCB_RVAL_CLOSE_CLIENT: Closes the client.
 * 		SCB_RVAL_STOP_SERVER: Closes all clients and stops the server.
 * 		SCB_RVAL_DEFAULT: Connects the client and adds it to the client
 * 			list.
 */
typedef server_cb_rval(*fscb_on_connect)(FdServer*, fds_package*);
/* Called after data has been received by a client.
 *
 * Parameters:
 * 		server: The server that received the message.
 *		client_package: The meta data about the client.
 * 		fd: The file descriptor received from the client.
 * 			This MUST be CLOSED by the user as the FdServer object
 * 			will not handle it.
 * Return Behavior:
 * 		SCB_RVAL_CLOSE_CLIENT: Closes the client.
 * 		SCB_RVAL_STOP_SERVER: Closes all clients and stops the server.
 * 		SCB_RVAL_DEFAULT: Continues waiting for data from other clients.
 */
typedef server_cb_rval(*fscb_on_receive)(FdServer*, fds_package*, int);
/* Called after a client is disconnected from the server.
 *
 * Parameters:
 * 		server: The server that the client is disconnecting from.
 * 		client_package: The meta data about the client.
 *
 * Return Behavior:
 * 		SCB_RVAL_STOP_SERVER: Closes all clients and stops the server.
 * 		SCB_RVAL_DEFAULT: Continues normal operations.
 */
typedef server_cb_rval(*fscb_on_disconnect)(FdServer*, fds_package*);
/*************************************/

/***************FdServer***************/
	/*******Public Functions*******/
/* Closes the given client that is connected to the server.
 *
 * If the package does not belong to the given server, the client
 * will not be closed.
 *
 * Parameters:
 * 		server: The server object that is related to 'sock'.
 * 		package: The client package belonging to the server.
 */
void FdServer_close_client(FdServer* server, fds_package* package);
/* Closes a client connected to the FdServer based on the given socket.
 * This should be called rather than 'close()' as it frees memory within
 * the FdServer.
 *
 * The client will be closed even if it is not found in the server,
 * it is highly suggested to use FdServer_close_client() instead.
 *
 * This is designed for when you don't have the fds_package for the client
 * or you are just not sure where the socket came from.
 *
 * Parameters:
 * 		server: The server object that is related to 'sock'.
 * 		sock: The socket of the client that should be closed.
 */
void FdServer_close_client_by_socket(FdServer* server, int sock);

/* Allocates then binds the socket.
 *
 * Returns:
 * 		ALIB_FD_ERROR: Error occurred during creation or binding of socket.
 *
 * Note:
 * 		If an error occurs, the FdServer's socket is guaranteed to be set to -1.
 */
alib_error FdServer_bind(FdServer* server);

/* Stops the FdServer.  This will not join the thread if async listening was used.
 *
 * To cleanup the thread after async listening was used, you must call
 * 'FdServer_stop_with_join()' or 'delFdServer()'. */
void FdServer_stop(FdServer* server);
/* Stops the FdServer.  If FdServer_run_on_thread() is used, this should not be
 * called within the listening thread, that includes the callback functions,
 * as the thread will attempt to join itself. If you must stop within the listening
 * loop, call 'FdServer_stop()' or, better yet, return the SCB_RVAL_STOP_SERVER.
 *
 * This function WILL BLOCK until the listening thread returns. */
void FdServer_stop_with_join(FdServer* server);

/* Starts the server listening.  If the server was already running,
 * it will be closed before restarting.
 *
 * This function WILL block until the the server is closed.
 *
 * Parameters:
 * 		server: The server that will be listening for clients.
 *
 * Note:
 * 		Upon return, 'server's socket is guaranteed to be -1. */
alib_error FdServer_run(FdServer* server);
/* Starts a thread.  If the server is already running, ALIB_OK will
 * be returned.
 *
 * Returns:
 * 		alib_error.
 * 		Anything else: Error code returned by pthread_create(). */
int FdServer_run_on_thread(FdServer* server);

		/* Getters */
/* Returns the socket for the server.
 *
 * Assumes server is not null. */
int FdServer_get_socket(FdServer* server);
/* Returns the ArrayList of client sockets connected to the server.
 *
 * Assumes server is not null. */
const ArrayList* FdServer_get_client_sockets(FdServer* server);
/* Returns 0 if the server is not running, !0 otherwise.
 *
 * Assumes server is not null. */
char FdServer_is_running(FdServer* server);
/* Returns the list of flags used by the object.
 *
 * Assumes server is not null. */
flag_pole FdServer_get_flag_pole(FdServer* server);

/* Returns the extended data of the server.
 *
 * Assumes 'server' is not null. */
void* FdServer_get_extended_data(FdServer* server);

/* Returns the address struct for the FdServer.
 *
 * Assumes 'server' is not null. */
const struct sockaddr_un* FdServer_get_addr(FdServer* server);
		/***********/

		/* Setters */
/* Sets the on connect callback for the server.
 *
 * Assumes 'server' is not null. */
void FdServer_set_on_connect_cb(FdServer* server, fscb_on_connect on_connect);
/* Sets the on receive callback for the server.
 *
 * Assumes 'server' is not null. */
void FdServer_set_on_receive_cb(FdServer* server, fscb_on_receive on_receive);
/* Sets the on disconnect callback for the server.
 *
 * Assumes 'server' is not null. */
void FdServer_set_on_disconnect_cb(FdServer* server, fscb_on_disconnect on_disconnect);

/* Sets the extended data for the FdServer.
 *
 * Parameters:
 * 		server: The server who's extended data should be set.
 * 		extended_data: A pointer to the data that should be attached to
 * 			to the server.
 * 		free_extended_data_cb: The callback function used to free the
 * 			extended data.  This will be called whenever the server
 * 			object is destroyed or the extended data is is replaced.
 * 		free_old_data: If !0, the old extended_data will be freed before
 * 			moving on.  This only works if the the 'free_extended_data_cb'
 * 			had previously been set.
 *
 * Notes:
 * 		Assumes 'server' is not null.
 */
void FdServer_set_extended_data(FdServer* server, void* extended_data,
		alib_free_value free_extended_data_cb, char free_old_data);
		/***********/
	/******************************/

	/*******Constructors*******/
/* Creates a new FdServer.
 *
 * Parameters:
 * 		fname: This is the temporary file name that will be used
 * 			by the UNIX file descriptor.  Cannot be larger than
 * 			sizeof(struct sockaddr_un.sun_path) (108 bytes as of 2015/9/28).
 * 			If the size does exeed this ammount, the data will be truncated.
 * 		name_len: The length of 'fname'.  If 0, the value will be calculated by
 * 			strlen(fname).
 * 		on_connect: (OPTIONAL) Called whenever a client connects to the server.
 * 		on_receive: (OPTIONAL) Called whenever data is received from the client.
 *
 * Returns:
 * 		NULL: Error.
 * 		FdServer*: Success.
 */
FdServer* newFdServer(const char* fname, size_t name_len,
		fscb_on_connect on_connect, fscb_on_receive on_receive);

/* Deletes the FdServer and sets the pointer to NULL. */
void delFdServer(FdServer** server);
	/**************************/
/**************************************/

#endif
