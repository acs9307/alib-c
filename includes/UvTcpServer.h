#ifndef UV_TCP_SERVER_IS_INCLUDED
#define UV_TCP_SERVER_IS_INCLUDED

#include <uv.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <stdlib.h>

#include "server_defines.h"
#include "alib_error.h"
#include "ArrayList.h"
#include "MemPool.h"
#include "UvTcp.h"

/*******CLASS DECLARATIONS*******/
/* A wrapper object for creating a libuv tcp server.
 * This is a multi-threaded server that handles request by callbacks.
 * Though all callbacks have default behavior, users can modify
 * the behavior by handling the different callbacks. */
typedef struct UvTcpServer UvTcpServer;
/********************************/

/*******ENUMS*******/
/* Return codes that should be returned by the user's
 * callbacks. Depending on the return value depends on
 * the behavior of the server.
 *
 * The values are all considered requests that the server
 * can choose to ignore, depending on circumstances.
 *
 * The values are designed to be 'or'ed together
 * to form complex return codes. */
typedef enum uscb_rval
{
	/* Continues default operation. */
	USCB_DEFAULT = 0,
	/* Requests the client be closed. */
	USCB_CLIENT_CLOSE = 1,
	/* Requests the server be shutdown. */
	USCB_SERVER_SHUTDOWN = 2,
	/* Requests that the server not close handles. */
	USCB_CONTINUE = 4,
}uscb_rval;
/*******************/

/*******FUNCTION POINTER TYPES*******/
/* Called after a client is accepted by the server
 * but before the client is added to the server's client list.
 *
 * Parameters:
 * 		server: The server that the client connected to.
 * 		client_handle: The client that was accepted.
 *		data: (OUT)A double pointer that can be set to any data desired.
 * 			The value will be set to server by default and should
 * 			only be modified if other behavior is desired.  This value
 * 			will be passed to all client related callbacks.
 * 		free_data_proc: (OUT)The callback for freeing the user data.
 * 			This should only be set if the data was set to a related
 * 			value. */
typedef uscb_rval (*us_client_con)(UvTcpServer*, uv_tcp_t*, void**, alib_free_value*);
/* Called after data from a client has been received and the
 * quit command (if turned on) has been processed.
 *
 * Parameters:
 * 		server: The server that called the callback.
 * 		client_stream: A stream to the client.
 * 		data: Data received from the client.
 * 		data_len: The length of data in bytes.
 * 		user_data: The data passed to each callback set when
 * 			the client connects. Default behavior is to set
 * 			this to the related UvTcpServer.
 */
typedef uscb_rval (*us_data_in)(UvTcpServer*, uv_stream_t*, const char*, size_t, void*);
/* Called when the client is about to be closed.  If USCB_CONTINUE is returned,
 * the disconnection of the client can be prevented, but this is not guaranteed.
 *
 * Parameters:
 * 		server: The server that called the callback.
 * 		client: The handle to the client.
 * 		user_data: The data passed to each callback set when
 * 			the client connects.  Default behavior is to set
 * 			this to the related UvTcpServer.
 */
typedef uscb_rval (*us_client_discon)(UvTcpServer*, uv_handle_t*, void*);
/* Called when a signal event is raised, usually from SIGINT.
 *
 * Parameters:
 * 		server: The server that caught the signal event.
 *		sig: The signal raised, should always be SIGINT.
 */
typedef uscb_rval (*us_sigint_cb)(UvTcpServer*, int);
typedef uscb_rval (*us_quit_command_cb)(UvTcpServer*, uv_stream_t*);
/************************************/

/*******PUBLIC FUNCTIONS*******/
/* Starts a UvTcpServer running.  This function will block
 * the calling thread until the server is shutdown.
 *
 * Returns:
 * 		alib_error: Error occurred.
 * 		Anything else: Return code of 'uv_run()'.
 */
int UvTcpServer_start(UvTcpServer* server);
/* Stops the server.
 *
 * Parameters:
 * 		server: The UvTcpServer to stop.
 */
void UvTcpServer_stop(UvTcpServer* server);

/* Adds a client from an already open socket. The server's connection_cb
 * will be called on the client after it has been added.
 *
 * Parameters:
 * 		server: The UvTcpServer that the client should be added to.
 * 		sock: The socket of the client that will be added to the server.
 */
int UvTcpServer_add_client(UvTcpServer* server, int sock);
/* Removes a client from the server and closes the client's socket.
 *
 * This is safe to call within the callback functions, however it
 * is still suggested to use the USCB_CLIENT_CLOSE return code instead.
 *
 * Parameters:
 * 		server: The server the client belongs to.
 * 		client: The client to remove from the server.
 */
void UvTcpServer_remove_client(UvTcpServer* server, uv_handle_t* client);

	/* Getters */
/* Returns !0 if the server is running, 0 otherwise.
 *
 * Assumes server is not null. */
char UvTcpServer_is_running(UvTcpServer* server);
/* Gets the backlog size of the server.
 *
 * Assumes the server is not null. */
size_t UvTcpServer_get_backlog_size(UvTcpServer* server);
/* Gets the extended data of the server.
 *
 * Assumes the server is not null. */
void* UvTcpServer_get_extended_data(UvTcpServer* server);

/* Returns the number of clients connected to the server.
 *
 * Assumes 'server' is not null. */
size_t UvTcpServer_get_client_count(UvTcpServer* server);
/* Returns !0 if the server will handle quit commands,
 * 0 otherwise.
 *
 * Assumes 'server' is not null.
 */
char UvTcpServer_will_handle_quit(UvTcpServer* server);
/* Returns the quit command that will be handled by the server if
 * the handle quit command flag is raised.
 *
 * Assumes 'server' is not null. */
const char* UvTcpServer_get_quit_command(UvTcpServer* server);

/* Returns the number of input buffers that have been allocated.
 *
 * Assumes server is not null. */
size_t UvTcpServer_get_input_buff_count(UvTcpServer* server);

/* Returns the address struct for the server.
 *
 * Assumes server is not null. */
const struct sockaddr_in* UvTcpServer_get_addr(UvTcpServer* server);
/* Returns the loop of the UvTcpServer.
 *
 * Assumes server is not null.
 */
const uv_loop_t* UvTcpServer_get_loop(UvTcpServer* server);
	/***********/

	/* Setters */
/* Sets the server's client_con_cb to the given callback.
 *
 * Assumes server is not null. */
void UvTcpServer_set_client_con_cb(UvTcpServer* server, us_client_con client_con_cb);
/* Sets the server's data_in_cb to the given callback.
 *
 * Assumes server is not null. */
void UvTcpServer_set_data_in_cb(UvTcpServer* server, us_data_in data_in_cb);
/* Sets the server's client_discon_cb to the given callback.
 *
 * Assumes server is not null. */
void UvTcpServer_set_client_discon_cb(UvTcpServer* server,
		us_client_discon client_discon_cb);
/* Sets the server's sigint callback.
 *
 * Assumes server is not null. */
void UvTcpServer_set_sigint_cb(UvTcpServer* server,
		us_sigint_cb sigint_cb);
void UvTcpServer_set_quit_command_cb(UvTcpServer* server,
		us_quit_command_cb qc_cb);

/* Sets the quit command for the server.
 * If the quit command is turned on and the server receives
 * a quit message, then the data_in_cb will not be called.
 *
 * Parameters:
 * 		server: The server whose quit command should be set.
 * 		handle_quit_cmd: If !0, the following quit command will be
 * 			used to determine if the client sent a shutdown request to
 * 			the server.  If 0, this will not be automatically handled.
 * 		quit_cmd: The command to listen for.  If NULL, then the
 * 			DEFAULT_QUIT_COMMAND will be used. */
void UvTcpServer_set_quit_command(UvTcpServer* server, char handle_quit_cmd,
		const char* quit_cmd);
/* Sets the backlog size of the server.  If the server is already running,
 * it will change the value then restart the server. */
void UvTcpServer_set_backlog_size(UvTcpServer* server, size_t backlog_size);
/* Sets the maximum number of input buffers allowed to be allocated at one
 * time.  By default, this is set to DEFAULT_BACKLOG_SIZE.
 *
 * Assumes server is not null.
 */
void UvTcpServer_set_max_input_buffer_cap(UvTcpServer* server, size_t max_size);

/* Sets the extended data for the server.  If the server already contained
 * extended data, the server will first try to free it.
 *
 * Parameters:
 * 		server: The server to add the extended data to.
 * 		extended_data: The data to be added to the server.
 * 		free_extended_data: (OPTIONAL) The callback used to free
 * 			the extended data. If no callback is needed, set to NULL.
 */
void UvTcpServer_set_extended_data(UvTcpServer* server, void* extended_data,
		alib_free_value free_extended_data);
	/***********/
/******************************/

/*******CONSTRUCTORS*******/
/* Creates a new UvTcpServer with default initialization.  For more
 * complete construction, use newUvTcpServer_ex().
 *
 * Parameters:
 * 		port: The port to listen on.
 * 		handle_quit_command: If 1, the server will automatically handle
 * 			incoming quit commands.  If a quit command is received
 * 			the 'data_in_cb' will not be called.  To change the command
 * 			value searched for, use UvTcpServer_set_quit_command.
 *
 * Returns:
 * 		UvTcpServer*: Newly allocated server.
 * 		NULL: Error occurred during initialization.
 */
UvTcpServer* newUvTcpServer(uint16_t port, char handle_quit_command);
/* Deletes the server and sets the pointer to NULL.
 * If the server is running, it will be shutdown.
 *
 * Parameters:
 * 		server: UvTcpServer to destroy.  If successfully
 * 			destroyed, the pointer will be set to NULL.
 */
void delUvTcpServer(UvTcpServer** server);
/**************************/
#endif
