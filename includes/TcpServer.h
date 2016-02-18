#ifndef TCP_SERVER_IS_DEFINED
#define TCP_SERVER_IS_DEFINED

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/epoll.h>
#include <errno.h>

#include "alib_cb_funcs.h"
#include "alib_error.h"
#include "alib_types.h"
#include "ArrayList.h"
#include "EpollPack.h"
#include "flags.h"
#include "server_defines.h"
#include "server_structs.h"

/* Simple TcpServer object used to handle incoming TCP connections.
 * Listening can be done either on a single thread or on a second thread.
 *
 * All callbacks run on the same thread. */
typedef struct TcpServer TcpServer;

/*******Callback Defines*******/
/* Called whenever a client connects to the server.
 *
 * Parameters:
 * 		server - The server that the event occurred on.
 * 		client - A socket_package struct containing data
 * 			related to the client.
 *
 * Return Value Behavior:
 * 		SCB_RVAL_CLOSE_CLIENT - Closes the client.
 * 		SCB_RVAL_STOP_SERVER - Closes the client and stops the server.
 * 		SCB_RVAL_HANDLED - Assumes that the client has been handled completely
 * 			and will not add the client to the client list nor add it to the
 * 			epoll array.
 * 		SCB_RVAL_DEFAULT - Adds the client to the client list and the epoll list. */
typedef server_cb_rval (*ts_client_connected_cb)(TcpServer* server,
		socket_package* client);
/* Called whenever data is ready to be read from a client socket.
 *
 * This usually should only be used if something other than 'recv()' is to be called.
 *
 * Parameters:
 * 		server - The server that received the data ready event.
 * 		client - The socket package describing the client who sent the data.
 * 		out_data - The buffer that is used to store the client's data.  This
 * 			is internally allocated, but can be modified as needed, though if
 * 			modified, the user MUST always handle allocation of the buffer and
 * 			reading from the client. The buffer will be freed when the server is
 * 			stopped.  If freed by the user, the value MUST be set to NULL or a
 * 			double free will occur.
 * 		out_data_len - The number of bytes received from the client.
 *
 * Return Value Behavior:
 * 		SCB_RVAL_CLOSE_CLIENT - Closes a client and removes it from the client list.
 * 		SCB_RVAL_STOP_SERVER - Closes the client and stops the server.
 * 		SCB_RVAL_HANDLED - Assumes that everything was handled and will not
 * 			call ts_client_data_in_cb.
 * 		SCB_RVAL_DEFAULT - If the data read count is negative, then the client will
 * 			be closed, otherwise it will call the client_data_in callback.
 */
typedef server_cb_rval (*ts_client_data_ready_cb)(TcpServer* server,
		socket_package* client, char** out_data, int* out_data_len);
/* Called whenever data is received from a client socket.
 *
 * Parameters:
 * 		server - The server that received the data.
 * 		client - The client that sent the data.
 * 		buff - The buffer that is storing the data.
 * 		buff_len - The number of bytes received from the client.
 *
 * Return Value Behavior:
 * 		SCB_RVAL_CLOSE_CLIENT - Closes the client and removes it from the list.
 * 		SCB_RVAL_STOP_SERVER - Stops the server and closes the client and removes
 * 			it from the list.
 * 		SCB_DEFAULT - Nothing.
 */
typedef server_cb_rval (*ts_client_data_in_cb)(TcpServer* server,
		socket_package* client, char* buff, size_t buff_len);
/* Called whenever a client disconnects from the server.
 *
 * Parameters:
 * 		server - A pointer to the calling object.
 * 		client - The client that disconnected.
 *
 * Return Value Behavior:
 * 		SCB_RVAL_STOP_SERVER - Closes all connected clients and stops the server.
 * 		SCB_DEFAULT - Nothing. */
typedef server_cb_rval (*ts_client_disconnected_cb)(TcpServer* server,
		socket_package* client);
/* Called whenever the thread is about to return. Only called when running
 * in async mode. */
typedef void (*ts_thread_returning_cb)(TcpServer* server);
/******************************/

/*******Public Functions*******/
/* Starts the TcpServer on the current thread.  If the server is already
 * running, it will first be stopped then restarted. To prevent this behavior, first
 * check 'TcpServer_is_running()'. */
alib_error TcpServer_start(TcpServer* server);
/* Starts the TcpServer on a separate thread.
 * If the server is already running, ALIB_OK is returned. */
alib_error TcpServer_start_async(TcpServer* server);

/* Stops the TcpServer. */
void TcpServer_stop(TcpServer* server);

/* Getters */
/* Returns the socket of the server.
*
* Assumes 'server' is not null. */
int TcpServer_get_sock(TcpServer* server);
/* Returns the sockaddr_in struct of the server.
 *
 * Though it is suggested not to modify this struct, sometimes the need may arise.
 * For modifications to be effective, the caller must make changes before starting
 * the server, otherwise behavior is undefined.
 *
 * Assumes 'server' is not null. */
const struct sockaddr_in* TcpServer_get_addr(TcpServer* server);
/* Returns 0 if the server is not running, otherwise !0.
*
* Assumes 'server' is not null. */
char TcpServer_is_running(TcpServer* server);
/* Returns the flag pole of the server.
*
* Assumes 'server' is not null. */
flag_pole TcpServer_get_flag_pole(TcpServer* server);
/* Returns a constant list of clients that are currently connected to the server.
*
* Assumes 'server' is not null. */
const ArrayList* TcpServer_get_client_list(TcpServer* server);
/* Returns the extended data of the server.
*
* Assumes 'server' is not null. */
void* TcpServer_get_extended_data(TcpServer* server);
/***********/

/* Setters */
/* Sets the callback for when a client connects to the server.
*
* Assumes 'server' is not null. */
void TcpServer_set_client_connected_cb(TcpServer* server,
	ts_client_connected_cb client_connected);
/* Sets the callback for when data is ready on a client socket.
*
* Assumes 'server' is not null. */
void TcpServer_set_client_data_ready_cb(TcpServer* server,
	ts_client_data_ready_cb client_data_ready);
/* Sets the callback for when data is received from a client.
*
* Assumes 'server' is not null. */
void TcpServer_set_client_data_in_cb(TcpServer* server,
	ts_client_data_in_cb client_data_in);
/* Sets the callback for when a client disconnects from the server.
*
* Assumes 'server' is not null. */
void TcpServer_set_client_disconnected_cb(TcpServer* server,
	ts_client_disconnected_cb client_disconnected);
/* Sets the callback for when the listening thread is about to return.
 *
 * Assumes 'server' is not null. */
void TcpServer_set_thread_returning_cb(TcpServer* server,
		ts_thread_returning_cb);

/* Sets the the extended data for the server.
*
* Assumes 'server' is not null.
*
* Parameters:
* 		server: The server to modify.
* 		ex_data: The data to set for the server's extended data.
* 		free_data: The callback function used to free the extended data when
* 			it is no longer needed.
* 		free_old_data: If !0, the server will automatically call the free_data_cb
* 			on the old extended data if possible. */
void TcpServer_set_extended_data(TcpServer* server, void* ex_data,
	alib_free_value free_data, char free_old_data);
	/***********/
/******************************/

/*******Constructors*******/
/* Creates a new TcpServer but does not start the server nor bind the server's socket.
 * To start the server, call TcpServer_start() or TcpServer_start_async(). */
TcpServer* newTcpServer(uint16_t port, void* ex_data,
		alib_free_value free_data_cb);
void delTcpServer(TcpServer** server);
/**************************/


#endif
