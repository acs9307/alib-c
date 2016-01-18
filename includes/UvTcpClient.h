#ifndef UV_TCP_CLIENT_IS_DEFINED
#define UV_TCP_CLIENT_IS_DEFINED

#include <uv.h>
#include <stdlib.h>
#include <inttypes.h>
#include <sys/socket.h>
#include <netdb.h>

#include "UvTcp.h"
#include "server_defines.h"
#include "alib_string.h"

/* VERY VERY SHITTY, DO NOT USE.
 *
 * Works fine on single thread, but can't be stopped from another thread.
 * If multi-threading is used, can only be stopped within the callbacks. */

/*******Class Declarations*******/
typedef struct UvTcpClient UvTcpClient;
/********************************/

/*******Enums*******/
typedef enum uccb_rval
{
	UCCB_DEFAULT = 0,
	UCCB_STAY_CONNECTED = 1,
	UCCB_DISCONNECT = 2,
}uccb_rval;
/*******************/

/*******Callback Function Pointers*******/
typedef void (*uc_client_connect_cb)(UvTcpClient*, int);
typedef void (*uc_client_close_cb)(UvTcpClient*);
/* Called when the client receives data.
 *
 * Parameters:
 * 		client: The client object that received data.
 * 		data: The data received.
 * 		data_len: The length of the data in bytes.
 */
typedef void (*uc_client_read_cb)(UvTcpClient*, const char*, size_t);
/****************************************/

/*******Public Functions*******/
/* Connects the client to its specified host.  This function will block until
 * the client disconnects.  For a non-blocking solution, use
 * UvTcpClient_connect_threaded().
 *
 * Parameters:
 * 		client: The client to connect to its host.
 */
alib_error UvTcpClient_connect(UvTcpClient* client);
/* Connects the client its specified host using a separate thread,
 * therefore this function will not block.  To stop the thread,
 * simple disconnect the client.
 *
 * Parameters:
 * 		client: The client to connect to its host.
 */
alib_error UvTcpClient_connect_threaded(UvTcpClient* client);

/* Disconnects the client from the host.
 *
 * Parameters:
 * 		client: The client object to be disconnected. */
void UvTcpClient_disconnect(UvTcpClient* client);

	/* Getters */
/* Returns !0 if connected and 0 if not connected.
*
* Assumes client is not null. */
char UvTcpClient_is_connected(UvTcpClient* client);
/* Returns the size of the receive buffer for the client.
 *
 * Assumes 'client' is not null. */
size_t UvTcpClient_get_recv_buff_size(UvTcpClient* client);
/* Returns the client's handle.
 *
 * Assumes 'client' is not null. */
const uv_tcp_t* UvTcpClient_get_handle(UvTcpClient* client);
/* Returns the socket for the client.
 *
 * Assumes 'client' is not null. */
int UvTcpClient_get_sock(UvTcpClient* client);

/* Returns 1 if the thread is running, 0 if it is not.
 * If UvTcpClient_connect_threaded(), the loop may still be
 * running, but only on the calling thread.  To ensure,
 * check 'UvTcpClient_is_connected()' to see if the
 * client loop is still running.
 *
 * Assumes 'client' is not null. */
char UvTcpClient_thread_is_running(UvTcpClient* client);
	/***********/

	/* Setters */
/* Sets the client connect callback for the UvTcpClient.
 *
 * Assumes 'client' is not null. */
void UvTcpClient_set_client_connect_cb(UvTcpClient* client,
		uc_client_connect_cb client_connect_cb);
/* Sets the client disconnect callback for the UvTcpClient.
 *
 * Assumes 'client' is not null. */
void UvTcpClient_set_client_disconnect_cb(UvTcpClient* client,
		uc_client_close_cb client_disconnect_cb);
/* Sets the client read callback for the UvTcpClient. */
void UvTcpClient_set_client_read_cb(UvTcpClient* client,
		uc_client_read_cb client_read_cb);
/* Sets the size for the client's receive buffer.  This can only be
 * set if the user is not currently connected or it does not currently have
 * a buffer.
 *
 * If the size must be changed during operation, it should be changed within
 * the read callback.
 *
 * Parameters:
 * 		client: The client to modify.
 * 		size: The new size for the client receive buffer. If size is set
 * 			to zero, the size will be modified to be 1.
 */
void UvTcpClient_set_recv_buff_size(UvTcpClient* client, size_t size);
	/***********/
/******************************/

/*******Constructors*******/
/* Creates a new UvTcpClient, but does not connect.  To connect
 * to the host, call 'UvTcpClient_connect'.
 *
 * Parameters:
 * 		addr: The host name, in ASCII form.
 * 		port: The port of the host applicaiton.
 *
 * Returns:
 * 		NULL: Error.
 * 		UvTcpClient*: The newly created client.
 */
UvTcpClient* newUvTcpClient(const char* addr, uint16_t port);
/* Deletes a UvTcpClient object and sets the pointer to NULL.
 * If the client is already connect, the client will first be disconnected before
 * freeing memory. */
void delUvTcpClient(UvTcpClient** client);
/**************************/

#endif
