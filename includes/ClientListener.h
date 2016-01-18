#ifndef CLIENT_LISTENER_IS_DEFINED
#define CLIENT_LISTENER_IS_DEFINED

#include <pthread.h>
#include <sys/epoll.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <errno.h>

#include "ArrayList.h"
#include "alib_cb_funcs.h"
#include "flags.h"
#include "server_defines.h"
#include "server_structs.h"

/* Object used for listening to a list of clients without having to open a new socket.  Basically
 * a server without using a port.  This is mainly used if the listening of sockets must be done one a
 * separate thread or in a separate instance from the server.  It is also useful if sockets are
 * being passed to your application from another application.
 *
 * NOTES:
 * 		When closing any related socket within a callback thread, DO NOT
 * 		call 'close()' unless you know what you are doing, ALWAYS use the
 * 		return value when possible.  If OUTSIDE a callback thread, use
 * 		the object's functions used for closing sockets whenever possible. */
typedef struct ClientListener ClientListener;

/*******Callback Function Types*******/
/* Called whenever data is ready to be read from a socket.  When possible, it is suggested,
 * at least at the moment (Version 1.0.0 11/17/2015), as the callback is not very safe. This should be fixed in the
 * future.
 *
 * Parameters:
 * 		listener: The ClientListener object that has a client with data ready for receiving.
 * 		client: The client that is ready to read from.
 * 		data: A double pointer to the buffer array.  This is where it gets dangerous
 * 			as the pointer will originally be allocated to a size of DEFAULT_INPUT_BUFF_SIZE.
 * 			This pointer can be set to anything, but remember you MUST FREE the memory currently
 * 			pointed to.  When the listening loop exits, the memory will be freed, if it is not set
 * 			to NULL.  Behavior is undefined when setting the pointer to NULL.
 * 		data_len: This should be set to the number of bytes received from the client.  If the
 * 			value is <0, then the client will be disconnected.
 * 			NOTE - This should not be used to purposely close the client.  It is safer and better
 * 				to return SERVER_CB_RVAL_CLOSE_CLIENT.
 *
 * Return Values:
 * 		SCB_RVAL_DEFAULT: Calls the listener's 'cl_client_data_in_cb' callback.
 * 		SCB_RVAL_CLOSE_CLIENT: Closes the client and continues waiting for incoming
 * 			data.
 * 		SCB_RVAL_STOP_SERVER: Stops the listener.
 * 		SCB_RVAL_HANDLED: Continues listening for new events without calling the
 * 			listener's 'cl_client_data_in_cb' on the client. */
typedef server_cb_rval (*cl_client_data_ready_cb)(ClientListener* listener, socket_package* client,
		void** data, long* data_len);
/* Called whenever data is received from the client.
 *
 * Parameters:
 * 		listener: The listener that received the event.
 * 		client: The client that sent the data.
 * 		data: The data received from the client.
 * 		data_len: The number of bytes received from the client.
 *
 * Return Behavior:
 * 		SCB_RVAL_DEFAULT: Continues listening.
 * 		SCB_RVAL_CLOSE_CLIENT: Closes the client.
 * 		SCB_RVAL_STOP_SERVER: Stops the listener. */
typedef server_cb_rval (*cl_client_data_in_cb)(ClientListener* listener, socket_package* client,
		void* data, size_t data_len);
/* Called whenever a client is disconnected from the listener.
 *
 * Return Behavior:
 * 		SCB_RVAL_DEFAULT: Nothing.
 * 		SCB_RVAL_STOP_SERVER: Stops the listener. */
typedef server_cb_rval (*cl_client_disconnected_cb)(ClientListener* listener, socket_package* client);
/* Called whenever the client list empties.  This will be called only once for each time the list empties.
 *
 * Return Behavior:
 * 		SCB_RVAL_DEFAULT:  Keeps the thread active until the 'ClientListener_stop()' is called.
 * 		SCB_RVAL_STOP_SERVER: Stops the listener. */
typedef server_cb_rval (*cl_client_list_empty_cb)(ClientListener* listener);
/*************************************/

/*******Public Functions*******/
/* Adds a socket package to the listener.
 *
 * Parameters:
 * 		listener: The object to modify.
 * 		client_pack: The socket package that should be added to
 * 			client list.  This WILL be FREED internally when the client
 * 			disconnects.  HOWEVER if an error occurs, the caller must
 * 			free the package, therefore it is suggested to use
 * 			'ClientListener_add()' instead.
 * 		run_asyc: If !0, ClientListener_start_async() will be called
 * 			before this function returns.
 */
alib_error ClientListener_add_socket_package(ClientListener* listener,
		socket_package* client_pack, char run_async);
/* Adds a socket to the ClientListener.  If an error occurs when trying to
 * add the client to the list, the provided socket WILL NOT be closed.
 *
 * Parameters:
 * 		listener: The object to modify.
 * 		sock: The socket to add to the listener.
 * 		run_async: If !0, 'ClientListener_start_async()' will be called
 * 			after adding the socket to the client list.
 * 		user_data: The user data to set for the client.
 * 		free_user_data: The callback function used to free the user
 * 			data when the client no longer needs it.
 */
alib_error ClientListener_add(ClientListener* listener, int sock,
		char run_async, void* user_data, alib_free_value free_user_data);

/* Removes the socket package from the listener and closes the socket.
 *
 * Assumes 'listener' and 'sp' is not null. */
void ClientListener_remove_socket_package(ClientListener* listener, socket_package* sp);
/* Removes a socket from the listener list, it is suggested to use
 * ClientListener_remove_socket_package() whenever possible as it
 * is much more efficient.
 *
 * Assumes 'listener' is not null. */
void ClientListener_remove(ClientListener* listener, int sock);

/* Starts listening for incoming events from the the object's clients.
 * This function BLOCKS until either 'ClientListener_stop()' is called
 * or all the clients have disconnected and there are no more clients in
 * the listener's client list.
 */
alib_error ClientListener_start(ClientListener* listener);
/* Starts clients in the ClientListener on a separate thread.  The
 * thread is managed internally and can be stopped by calling
 * 'ClientListener_stop()' or 'delClientListener()'.
 *
 * Returns:
 * 		ALIB_OK: Thread is already running, or thread was started without error.
 * 			Any error that occurs after this was raised in the listen loop.
 * 		Anything else: Error code related to the error.
 */
alib_error ClientListener_start_async(ClientListener* listener);

/* Stops a currently running ClientListener object.  If the object
 * is already stopped, nothing will happen.
 *
 * This should be called whenever the object is no longer running. */
alib_error ClientListener_stop(ClientListener* listener);
/* Waits until there are no more clients before calling ClientListener_stop()
 * on the object.  This will block until there are no more clients in the
 * list. */
alib_error ClientListener_stop_when_empty(ClientListener* listener);

	/* Getters */
/* Returns a constant pointer to the ClientListener's client list.
 * This should not be modified unless you know what you are doing as
 * it may corrupt the object.
 *
 * Clients are stored as 'socket_package' structs.
 *
 * Assumes 'listener' is not null. */
const ArrayList* ClientListener_get_client_list(ClientListener* listener);
/* Returns the extended data of the ClientListener object.
 *
 * Assumes 'listener' is not null. */
void* ClientListener_get_extended_data(ClientListener* listener);
/* Returns the flag pole of the ClientListener object.
 *
 * Assumes 'listener' is not null. */
object_flag_pole ClientListener_get_flag_pole(ClientListener* listener);
/* Returns the !0 if the object is listening for clients, 0 otherwise.
 *
 * Assumes 'listener' is not null. */
char ClientListener_is_listening(ClientListener* listener);
	/***********/

	/* Setters */
/* Sets the 'client_data_ready' callback for the ClientListener.
 *
 * Assumes 'listener' is not null. */
void ClientListener_set_client_data_ready_cb(ClientListener* listener,
		cl_client_data_ready_cb data_ready);
/* Sets the 'client_data_in' callback for the ClientListener.
 *
 * Assumes 'listener' is not null. */
void ClientListener_set_client_data_in_cb(ClientListener* listener,
		cl_client_data_in_cb data_in);
/* Sets the 'client_disconnected' callback for the ClientListener.
 *
 * Assumes 'listener' is not null. */
void ClientListener_set_client_disconnected_cb(ClientListener* listener,
		cl_client_disconnected_cb disconnected);
/* Sets the 'client_list_empty' callback for the ClientListener.
 *
 * Assumes 'listener' is not null. */
void ClientListener_set_client_list_empty_cb(ClientListener* listener,
		cl_client_list_empty_cb list_empty);

/* Sets the extended data for the ClientListener.
 *
 * Assumes 'listener' is not null.
 *
 * Parameters:
 * 		listener: The object to modify.
 * 		ex_data: The extended data for the listener.
 * 		free_ex_data: The callback to free 'ex_data' when it is no longer
 * 			needed.
 * 		free_old_data: If !0 and the listener already contains an extended
 * 			data object, it will be freed, if possible, before modifying the
 * 			pointer.
 */
void ClientListener_set_extended_data(ClientListener* listener,
		void* ex_data, alib_free_value free_ex_data, char free_old_data);
	/***********/

/******************************/

/*******Constructors*******/
/* Constructs a new ClientListener object.  To start the object, user should
 * call 'ClientListener_start()', 'ClientListener_start_async()', or
 * 'ClientListener_add()'.
 *
 * Memory should be freed by calling 'delClientListener()'.
 *
 * Parameters:
 * 		ex_data: The extended data for the object.
 * 		free_extended_data: The callback to free the extended data when
 * 			the object is finished with it.
 *
 * Returns:
 * 		NULL: Error occurred during construction.
 * 		ClientListener*: Newly allocated ClientListener object. */
ClientListener* newClientListener(void* ex_data, alib_free_value free_extended_data);

/* Destroys a ClientListener object and sets the pointer to NULL. */
void delClientListener(ClientListener** listener);
/**************************/


#endif
