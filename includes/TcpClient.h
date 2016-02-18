#ifndef TCP_CLIENT_IS_DEFINED
#define TCP_CLIENT_IS_DEFINED

#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <inttypes.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#include "alib_types.h"
#include "alib_error.h"
#include "server_defines.h"
#include "flags.h"

/*******Class Declaration*******/
typedef struct TcpClient TcpClient;
/*******************************/

/*******Callback Types*******/
/* Called whenever a client receives data from the host. */
typedef server_cb_rval(*tc_data_in)(TcpClient* client, const char* data, size_t data_len);
/* Called after a client's connection to the host has been lost. */
typedef server_cb_rval(*tc_disconnect)(TcpClient* client);
/* Called whenever a client is preparing to connect to a host, after a socket
 * has been created but before 'connect()' has been called.  This allows for
 * options to be set that will effect the socket during the connection process.
 *
 * Return Behavior:
 * 		 0: Success, continues normal operations.
 * 		!0: Aborts connecting to host.  It is suggested to return 'alib_error' codes.
 */
typedef int (*tc_sockopt)(TcpClient* client, int socket);
/****************************/

/*******Public Functions*******/
/* Connects a client to its host.
 *
 * Returns:
 * 		alib_error
 * 		Anything else is a return value from 'sockopt_cb'. */
int TcpClient_connect(TcpClient* client);
/* Disconnects the client from the host. */
void TcpClient_disconnect(TcpClient* client);

/* Sends the data to the client's host.
 * This function ensures that all data will be sent.  If not, an error code
 * will be returned.  Will BLOCK until all bytes are transmitted or error occurs.
 *
 * Parameters:
 * 		client: The client who will be sending the data.
 * 		data: The data to be sent.
 * 		data_len: The length of the data to be sent.
 *
 * Returns:
 * 		An alib_error that describes the error. */
alib_error TcpClient_send(TcpClient* client, const char* data, size_t data_len);

/* Starts the reading process on the client. */
void TcpClient_read_start(TcpClient* client);
/* Stops the reading process on the client. */
void TcpClient_read_stop(TcpClient* client);

	/* Getters */
/* Returns the user defined extended data of the client.
 *
 * Assumes client is not null.  */
void* TcpClient_get_ex_data(TcpClient* client);
/* Returns 1 if the client is connected, 0 otherwise.
 *
 * Assumes 'client' is not null. */
char TcpClient_is_connected(TcpClient* client);
/* Returns the socket for the client.
 *
 * Assumes client is not null. */
int TcpClient_get_socket(TcpClient* client);
/* Returns 1 if the client is currently reading, 0 otherwise.
 *
 * Assumes 'client' is not null. */
char TcpClient_is_reading(TcpClient* client);

/* Returns the client's ex_data and sets the client's pointer
 * to that data to NULL.  This will not call the free_data_cb.
 *
 * Assumes 'client' is not null. */
void* TcpClient_extract_ex_data(TcpClient* client);
	/***********/

	/* Setters */
/* Sets the data_in callback function for the client.  If the read
 * thread is not running, it will be started. */
void TcpClient_set_data_in_cb(TcpClient* client, tc_data_in data_in_cb);
/* Sets the disconnect cb for the client.
 * This callback is called after the client has been disconnected from the host.
 *
 * Assumes 'client' is not null. */
void TcpClient_set_disconnect_cb(TcpClient* client, tc_disconnect disconnect_cb);
/* Sets the sockopt cb for the client.
 * This callback is called whenever the client is about to connect to a host,
 * between the calls to 'socket()' and 'connect()'.
 *
 * Assumes 'client' is not null. */
void TcpClient_set_sockopt_cb(TcpClient* client, tc_sockopt sockopt_cb);

/* Sets the extended data for the client. If extended data already exists,
 * this will try to free the old data before replacing it.
 *
 * Assumes 'client' is not null. */
void TcpClient_set_ex_data(TcpClient* client, void* ex_data,
		alib_free_value free_data_cb);
	/***********/
/******************************/

/*******Constructors*******/
/* Creates a new TcpClient.
 *
 * Parameters:
 * 		host_addr: The address of the host.
 * 		port: The port of the host.
 * 		ex_data (OPTIONAL): Extended data for the client.
 * 		free_data_cb (OPTIONAL): Used to free the extended data of the client upon
 * 			object destruction.
 *
 * Returns:
 * 		NULL: Error.
 * 		TcpClient*: New TcpClient. */
TcpClient* newTcpClient(const char* host_addr, uint16_t port,
		void* ex_data, alib_free_value free_data_cb);
/* Creates a TcpClient from an already connected socket.
 *
 * Parameters:
 * 		sock: The socket to build the client from.
 *      close_sock_on_free: If !0, the socket will be closed upon object deletion.
 * 		ex_data (OPTIONAL): Extended data for the client.
 * 		free_data_cb (OPTIONAL): Used to free the extended data of the client upon
 * 			object destruction.
 *
 * Returns:
 * 		NULL: Error.
 * 		TcpClient*: New TcpClient.*/
TcpClient* newTcpClient_from_socket(int sock, char close_sock_on_free, void* ex_data, alib_free_value free_data_cb);

/* Disconnects the client and destroys the object. */
void delTcpClient(TcpClient** client);
/**************************/

#endif
