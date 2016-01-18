#ifndef EVENT_SERVER_IS_DEFINED
#define EVENT_SERVER_IS_DEFINED

#include <event.h>
#include <event2/listener.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "alib_error.h"
#include "alib_types.h"
#include "ArrayList.h"
#include "server_defines.h"
#include "alib_string.h"

/*******ENUMS*******/
typedef enum escb_rval
{
	ESCB_CONTINUE = 0,
	ESCB_CLIENT_CLOSE = 1,
	ESCB_SERVER_SHUTDOWN = 2,
}escb_rval;
/*******************/

/*******CLASS DEFINITIONS*******/
/* A single threaded server build on libevent.  This is good for
 * time when a single thread is sufficient for handling requests and
 * is very simple in that no mutexing is required. */
typedef struct EventServer EventServer;
/*******************************/

/*******FUNCTION POINTER TYPES*******/
/* Called when a client connects to the server.
 *
 * Parameters:
 * 		server: The event server that received the connection call.
 * 		bev: The bufferevent created by the connection.
 * 		sock: The socket of the client.
 * 		new_user_data: (OPTIONAL) This will be the object that is
 * 			passed as 'user_data' to all subsequent callback functions
 * 			for this client.  If NULL, the server will be passed. */
typedef escb_rval (*es_client_con)(EventServer*, struct bufferevent*, int, void**, alib_free_value*);
/* Called when data is received from a client.
 *
 * Parameters:
 * 		data: Data received from the client.
 * 		data_len: The length of data in bytes.
 * 		user_data: The data passed to each callback set when
 * 			the client connects.  This is usually an EventServer
 * 			object. */
typedef escb_rval (*es_data_in)(const char*, size_t, void*);
/* Called when an event occurs that is not a connection event.
 *
 * Parameters:
 * 		bev: The bufferevent that the connection event occurred on.
 * 		events: The event raised that called this callback.
 * 		user_data: The data passed to each callback set when
 * 			the client connects.  This is usually an EventServer
 * 			object. */
typedef escb_rval (*es_client_discon)(struct bufferevent*, short, void*);
/* Called when a signal event is raised, usually from SIGINT.
 *
 * Parameters:
 *		sig: The signal raised.
 *		event: The events raised by the signal.
 * 		server: The server that caught the signal event.
 */
typedef escb_rval (*es_sig)(int, short, EventServer*);
/************************************/

/*******PUBLIC FUNCTIONS*******/
/* Starts the server on the current thread.  This will block
 * until the server is stopped. */
void EventServer_start(EventServer* server);
/* Stops the server's execution. */
void EventServer_stop(EventServer* server);

	/* GETTERS */
/* Returns the object's base.
 * Assumes 'server' is not null. */
const struct event_base* EventServer_get_base(EventServer* server);
/* Returns a pointer to the object's sockaddr_in struct.
 * This is the address data of the server.
 *
 * Assumes 'server' is not null. */
const struct sockaddr_in* EventServer_get_sin(EventServer* server);
/* Returns the object's connection listener member.
 *
 * Assumes 'server' is not null. */
const struct evconnlistener* EventServer_get_listener(EventServer* server);
/* Returns a the object's signal_event member.
 *
 * Assumes 'server' is not null. */
const struct event* EventServer_get_signal_event(EventServer* server);
/* Returns 1 if the server is running, 0 otherwise.
 *
 * Assumes 'server' is not null. */
const char EventServer_is_running(EventServer* server);

		/* Callbacks */
/* Returns the object's client_con_cb.
 * This is called when a client connects to the server.
 *
 * Assumes 'server' is not null. */
const es_client_con EventServer_get_client_con_cb(EventServer* server);
/* Returns the object's data_in_cb.
 * This is called when the server receives data from a client.
 *
 * Assumes 'server' is not null. */
const es_data_in EventServer_get_data_in_cb(EventServer* server);
/* Returns the object's client_discon_cb.
 * This is called when a client disconnects.
 *
 * Assumes 'server' is not null. */
const es_client_discon EventServer_get_client_discon_cb(EventServer* server);
/* Returns the object's sig_cb.
 * This is called when a signal event is received, usually SIGINT.
 *
 * Assumes 'server' is not null. */
const es_sig EventServer_get_sig_cb(EventServer* server);
		/*************/

/* Returns the value of the 'handle_quit_command' member of the server.
 * If the value is 0, then quit commands will not be automatically handled.
 * If it is !0, then quit commands are automatically handled.
 *
 * Assumes 'server' is not null. */
const char EventServer_will_handle_quit_command(EventServer* server);
/* Returns quit command that will be used by the server.
 * If a match is found, the server will shutdown without calling
 * the 'data_in_cb'.
 *
 * Assumes 'server' is not null. */
const char* EventServer_get_quit_command(EventServer* server);

/* Returns the server's user data.  This is used to expand
 * the server's data so that user defined data may be used
 * within the callback functions.
 *
 * Assumes 'server' is not null. */
const void* EventServer_get_extended_data(EventServer* server);
/* Returns the server's free_user_data function pointer.
 * If not null, then this will be called to free the 'user_data'
 * member upon server destruction.
 *
 * Assumes 'server' is not null. */
const alib_free_value EventServer_get_free_extended_data_proc(EventServer* server);
	/***********/

	/* SETTERS */
		/* Callbacks */
/* Sets the client connection callback for the server.
 *
 * Assumes server is not null.
 */
void EventServer_set_client_con_cb(EventServer* server, es_client_con client_con_cb);
/* Sets the data in callback for the server.
 *
 * Assumes server is not null.
 */
void EventServer_set_data_in_cb(EventServer* server, es_data_in data_in_cb);
/* Sets the client disconnect callback for the server.
 *
 * Assumes server is not null.
 */
void EventServer_set_client_discon_cb(EventServer* server,
		es_client_discon client_discon_cb);
/* Sets the client signal callback for the server.
 *
 * Assumes server is not null.
 */
void EventServer_set_sig_cb(EventServer* server, es_sig sig_cb);
		/*************/

/* Sets the value of the handle quit command of the server.
 *
 * Assumes server is not null.
 */
void EventServer_handle_quit_command(EventServer* server, char handle_quit_command);
/* Sets the value of the quit command for the server.  The string will be copied
 * to internal memory which will be deallocated automatically.
 *
 * Assumes server is not null.
 */
void EventServer_set_quit_command(EventServer* server, const char* quit_command);

/* Sets the server's user data.
 *
 * Assumes server is not null.
 */
void EventServer_set_extended_data(EventServer* server, void* extended_data);
/* Sets the server's function pointer for freeing the user data member.
 *
 * Assumes server is not null.
 */
void EventServer_set_free_extended_data_proc(EventServer* server,
		alib_free_value free_extended_data);
	/***********/
/******************************/

/*******CONSTRUCTORS*******/
/* Creates a new event server based on the passed parameters.
 * If an error occurs, the provided 'user_data' will NOT be deleted.
 *
 * Parameters:
 * 		out_server: The pointer that will be set to the new instance of
 * 			the EventServer.  If the pointer is already pointing to an
 * 			EventServer, the server will NOT be freed.
 *		port: The port to listen for clients on.
 *		client_con_cb: (OPTIONAL) The callback function that will be called when
 *			clients connect to the server.
 *		data_in_cb: (OPTIONAL) The callback function that is called when data
 *			is received from a client.
 *		client_discon_cb: (OPTIONAL) The callback function that is called when
 *			the client disconnects from the server.
 *		sig_cb: (OPTIONAL) The callback called when a signal (usually SIGINT) is
 *			called.
 *		extended_data: (OPIONAL) By default (though this can be changed by 'client_con_cb'),
 *			the server is passed as the 'user_data' argument of each callback function.
 *			This data will be appended to the server so that extra data is available
 *			to callbacks as needed.
 *		free_extended_data: (OPTIONAL) The function used to free the 'user_data' portion
 *			of the event server.
 *		handle_quit_command: If an automatic handling of the quit command is desired, set
 *			the value to !0, otherwise set it to 0.  The DEFAULT_QUIT_COMMAND will be used
 *			unless changed by 'EventServer_set_quit_command()'.
 */
alib_error newEventServer(EventServer** out_server, uint16_t port,
		es_client_con client_con_cb, es_data_in data_in_cb,
		es_client_discon client_discon_cb, es_sig sig_cb, void* extended_data,
		alib_free_value free_extended_data, char handle_quit_command);
/* Destroys an event server.  'free_user_data', if not null,
 * will be called on the 'user_data'. */
void delEventServer(struct EventServer** server);
/**************************/

#endif
