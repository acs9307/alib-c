#include "EventServer_private.h"

/*******CALLBACK PACKAGE*******/
/* Frees a callback package.
 * Type: 'alib_free_value'
 */
static void free_callback_package(void* void_package)
{
	escb_package* package = (escb_package*)void_package;

	if(package)
	{
		if(package->free_data_proc && package->data)
			package->free_data_proc(package->data);
		free(package);
	}
}
/******************************/

/*******PRIVATE MAIN CALLBACK FUNCTIONS*******/
/* Receives data from the user, converts it to a c-string, if desired,
 * checks message for a quit command, then calls the user callback function.
 * Memory for the c-string is handled internally and should not be freed by
 * the user.
 *
 * When callback returns, handles callback request. */
static void conn_readcb(struct bufferevent *bev, void *user_data)
{
	escb_package* package = user_data;
	EventServer* server = package->server;

	/* Convert bufferevent buffer to c-string. */
	struct evbuffer* recv_buff = bufferevent_get_input(bev);
	int recv_data_len = evbuffer_get_length(recv_buff);
	char* recv_data = malloc(recv_data_len);
	evbuffer_remove(recv_buff, recv_data, recv_data_len);

	/* Check for quit command. */
	if(server->handle_quit_command && recv_data_len >= server->quit_command_len &&
			str_match(recv_data, 0,	(server->quit_command)?server->quit_command:DEFAULT_QUIT_COMMAND, 0) == 0)
	{
		event_base_loopexit(server->base, NULL);
	}
	/* Call user callback. */
	else if(server->data_in_cb)
	{
		escb_rval r_val = server->data_in_cb(recv_data,
				recv_data_len, package->data);
		if(r_val & ESCB_CLIENT_CLOSE)
		{
			ArrayList_remove(server->callback_packages, package);
			bufferevent_free(bev);
		}
		if(r_val & ESCB_SERVER_SHUTDOWN)
			EventServer_stop(server);
	}

	/* Free the input buffer. */
	if(recv_data)
		free(recv_data);
}

/* Called whenever an event occurs with a client.  Any event that is not
 * BEV_EVENT_CONNECTED, this function will call the user's 'client_discon_cb'.
 * If no callback is set for the client, then the client is disconnected if the
 * event was not a connection event.
 *
 * When callback returns, handles callback request. */
static void conn_eventcb(struct bufferevent *bev, short events,
		void *user_data)
{
	escb_package* package = user_data;
	EventServer* server = package->server;

	if(events & BEV_EVENT_CONNECTED)
		return;
	else
	{
		/* Run user functions as needed. */
		if(server->client_discon_cb)
		{
			escb_rval r_val = server->client_discon_cb(bev, events, package->data);
			if(r_val & ESCB_CLIENT_CLOSE)
			{
				ArrayList_remove(server->callback_packages, package);
				bufferevent_free(bev);
			}
			if(r_val & ESCB_SERVER_SHUTDOWN)
				EventServer_stop(server);
		}
		/* Defaults to close client socket. */
		else
			bufferevent_free(bev);
	}
}

/* Called whenever a client connects to the server.  After the connection is
 * made, but before the bufferevent callback is made,
 * the client's 'client_con_cb' is called.
 *
 * When callback returns, handles callback request. */
static void listener_cb(struct evconnlistener* listener, evutil_socket_t fd,
		struct sockaddr* sa, int socklen, void* user_data)
{
	EventServer* server = (EventServer*)user_data;
	struct bufferevent* bev;

	escb_package* package = malloc(sizeof(escb_package));
	if(!package)return;
	package->server = server;

	/* Create the socket. */
	bev = bufferevent_socket_new(server->base, fd, BEV_OPT_CLOSE_ON_FREE);
	if(!bev)
	{
		event_base_loopbreak(server->base);
		return;
	}

	/* Run user event functions as needed. */
	if(server->client_con_cb)
	{
		escb_rval r_val = server->client_con_cb(server, bev, fd, &package->data, &package->free_data_proc);
		if(r_val & ESCB_CLIENT_CLOSE)
		{
			bufferevent_free(bev);

			/* Free the package if we need to. */
			if(package->free_data_proc && package->data)
				package->free_data_proc(package->data);
			return;
		}
		if(r_val & ESCB_SERVER_SHUTDOWN)
		{
			bufferevent_free(bev);
			EventServer_stop(server);

			/* Free the package if we need to. */
			if(package->free_data_proc && package->data)
				package->free_data_proc(package->data);
			return;
		}
	}
	else
		package->data = server;

	/* We have a new package that will have to be freed later on
	 * by our server.  Place it into the array list to ensure
	 * that there will be no memory leak when the server shuts down. */
	if(package->free_data_proc)
		ArrayList_add(server->callback_packages, package);

	/* Do the setup needed before returning. */
	bufferevent_setcb(bev,
			(server->data_in_cb || server->handle_quit_command)?conn_readcb:NULL,
			NULL, conn_eventcb, package);
	bufferevent_enable(bev, EV_WRITE);
	bufferevent_enable(bev, EV_READ);
}

/* Handle IO signals, particularly SIGINT.  If there is a user defined callback for
 * 'sig_cb', then it will be called and it's return value will be handled as
 * needed.
 *
 * If 'sig_cb' is NULL, then the server will be shutdown. */
static void signal_cb(evutil_socket_t sig, short events,void* user_data)
{
	EventServer* server = user_data;

	/* Call the user provided callback. */
	if(server->sig_cb)
	{
		escb_rval r_val = server->sig_cb(sig, events, server);
		if(r_val & ESCB_SERVER_SHUTDOWN)
			EventServer_stop(server);
	}
	/* No callback given, default is to stop the server. */
	else
		EventServer_stop(server);
}
/*********************************************/

/*******PUBLIC FUNCTIONS*******/
/* Starts the server on the current thread.  This will block
 * until the server is stopped. */
void EventServer_start(EventServer* server)
{
	if(server)
	{
		server->is_running = 1;
		event_base_dispatch(server->base);
	}
}
/* Stops the server's execution. */
void EventServer_stop(EventServer* server)
{
	if(server)
	{
		event_base_loopexit(server->base, NULL);
		server->is_running = 0;
	}
}

	/* GETTERS */
/* Returns the object's base.
 * Assumes 'server' is not null. */
const struct event_base* EventServer_get_base(EventServer* server)
{
	return(server->base);
}
/* Returns a pointer to the object's sockaddr_in struct.
 * This is the address data of the server.
 *
 * Assumes 'server' is not null. */
const struct sockaddr_in* EventServer_get_sin(EventServer* server)
{
	return(&server->sin);
}
/* Returns the object's connection listener member.
 *
 * Assumes 'server' is not null. */
const struct evconnlistener* EventServer_get_listener(EventServer* server)
{
	return(server->listener);
}
/* Returns a the object's signal_event member.
 *
 * Assumes 'server' is not null. */
const struct event* EventServer_get_signal_event(EventServer* server)
{
	return(server->signal_event);
}
/* Returns 1 if the server is running, 0 otherwise.
 *
 * Assumes 'server' is not null. */
const char EventServer_is_running(EventServer* server){return(server->is_running);}

		/* Callbacks */
/* Returns the object's client_con_cb.
 * This is called when a client connects to the server.
 *
 * Assumes 'server' is not null. */
const es_client_con EventServer_get_client_con_cb(EventServer* server)
{
	return(server->client_con_cb);
}
/* Returns the object's data_in_cb.
 * This is called when the server receives data from a client.
 *
 * Assumes 'server' is not null. */
const es_data_in EventServer_get_data_in_cb(EventServer* server)
{
	return(server->data_in_cb);
}
/* Returns the object's client_discon_cb.
 * This is called when a client disconnects.
 *
 * Assumes 'server' is not null. */
const es_client_discon EventServer_get_client_discon_cb(EventServer* server)
{
	return(server->client_discon_cb);
}
/* Returns the object's sig_cb.
 * This is called when a signal event is received, usually SIGINT.
 *
 * Assumes 'server' is not null. */
const es_sig EventServer_get_sig_cb(EventServer* server)
{
	return(server->sig_cb);
}
		/*************/

/* Returns the value of the 'handle_quit_command' member of the server.
 * If the value is 0, then quit commands will not be automatically handled.
 * If it is !0, then quit commands are automatically handled.
 *
 * Assumes 'server' is not null. */
const char EventServer_will_handle_quit_command(EventServer* server)
{
	return(server->handle_quit_command);
}
/* Returns quit command that will be used by the server.
 * If a match is found, the server will shutdown without calling
 * the 'data_in_cb'.
 *
 * Assumes 'server' is not null. */
const char* EventServer_get_quit_command(EventServer* server)
{
	if(!server->quit_command)
		return(DEFAULT_QUIT_COMMAND);
	else
		return(server->quit_command);
}

/* Returns the server's user data.  This is used to expand
 * the server's data so that user defined data may be used
 * within the callback functions.
 *
 * Assumes 'server' is not null. */
const void* EventServer_get_extended_data(EventServer* server)
{
	return(server->extended_data);
}
/* Returns the server's free_user_data function pointer.
 * If not null, then this will be called to free the 'user_data'
 * member upon server destruction.
 *
 * Assumes 'server' is not null. */
const alib_free_value EventServer_get_free_extended_data_proc(EventServer* server)
{
	return(server->free_extended_data);
}
	/***********/

	/* SETTERS */
		/* Callbacks */
/* Sets the client connection callback for the server.
 *
 * Assumes server is not null.
 */
void EventServer_set_client_con_cb(EventServer* server, es_client_con client_con_cb)
{
	server->client_con_cb = client_con_cb;
}
/* Sets the data in callback for the server.
 *
 * Assumes server is not null.
 */
void EventServer_set_data_in_cb(EventServer* server, es_data_in data_in_cb)
{
	server->data_in_cb = data_in_cb;
}
/* Sets the client disconnect callback for the server.
 *
 * Assumes server is not null.
 */
void EventServer_set_client_discon_cb(EventServer* server,
		es_client_discon client_discon_cb)
{
	server->client_discon_cb = client_discon_cb;
}
/* Sets the client signal callback for the server.
 *
 * Assumes server is not null.
 */
void EventServer_set_sig_cb(EventServer* server, es_sig sig_cb)
{
	server->sig_cb= sig_cb;
}
		/*************/

/* Sets the value of the handle quit command of the server.
 *
 * Assumes server is not null.
 */
void EventServer_handle_quit_command(EventServer* server, char handle_quit_command)
{
	server->handle_quit_command = handle_quit_command;
}
/* Sets the value of the quit command for the server.  The string will be copied
 * to internal memory which will be deallocated automatically.
 *
 * Assumes server is not null.
 */
void EventServer_set_quit_command(EventServer* server, const char* quit_command)
{
	/* Free the memory of the previous quit command. */
	if(server->quit_command != NULL)
		free(server->quit_command);

	/* If the given command is not null, then allocate new memory and copy the
	 * data over. */
	if(quit_command)
	{
		server->quit_command_len = strlen(quit_command);
		strncpy_alloc(&server->quit_command, quit_command, server->quit_command_len);
	}
	else
		server->quit_command = NULL;
}

/* Sets the server's user data.
 *
 * Assumes server is not null.
 */
void EventServer_set_extended_data(EventServer* server, void* extended_data)
{
	server->extended_data = extended_data;
}
/* Sets the server's function pointer for freeing the user data member.
 *
 * Assumes server is not null.
 */
void EventServer_set_free_extended_data_proc(EventServer* server,
		alib_free_value free_extended_data)
{
	server->free_extended_data = free_extended_data ;
}
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
		alib_free_value free_extended_data, char handle_quit_command)
{
	alib_error err = ALIB_OK;

	/* Check for errors. */
	if(!out_server)return(ALIB_BAD_ARG);

	/* Allocate memory for server object. */
	if(!(*out_server = malloc(sizeof(EventServer))))
		return(ALIB_MEM_ERR);

	/* Setup the base. */
	if(!((*out_server)->base = event_base_new()))
	{
		free(*out_server);
		*out_server = NULL;
		return(ALIB_MEM_ERR);
	}

	/* Setup listener callback. */
	memset(&(*out_server)->sin, 0, sizeof((*out_server)->sin));
	(*out_server)->sin.sin_family = AF_INET;
	(*out_server)->sin.sin_port = htons(port);
	(*out_server)->listener = evconnlistener_new_bind((*out_server)->base, listener_cb,
			(void*)(*out_server),LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, -1,
			(struct sockaddr*)&(*out_server)->sin, sizeof((*out_server)->sin));

	/* Setup signal event. */
	(*out_server)->signal_event = evsignal_new((*out_server)->base, SIGINT, signal_cb,
			(void*)(*out_server));
	event_add((*out_server)->signal_event, NULL);

	/* Add the quit command. */
	(*out_server)->quit_command = NULL;
	(*out_server)->quit_command_len = strlen(DEFAULT_QUIT_COMMAND);
	(*out_server)->handle_quit_command = handle_quit_command;

	/* Check for errors. */
	if(!(*out_server)->listener || !(*out_server)->signal_event)
	{
		if(!(*out_server)->listener)
			err = ALIB_PORT_IN_USE;
		else
			err = ALIB_UNKNOWN_ERR;

		/* Set the uninitialized members to NULL before deleting. */
		(*out_server)->free_extended_data  = NULL;
		goto f_return;
	}

	/* Initialize callback packages list. */
	(*out_server)->callback_packages = newArrayList_ex(free_callback_package, 0, SIZE_MAX, 0);

	/* Initialize callbacks. */
	(*out_server)->client_con_cb = client_con_cb;
	(*out_server)->data_in_cb = data_in_cb;
	(*out_server)->client_discon_cb = client_discon_cb;
	(*out_server)->sig_cb = sig_cb;

	/* Initialize other data members. */
	(*out_server)->is_running = 0;
	(*out_server)->extended_data = extended_data;
	(*out_server)->free_extended_data  = free_extended_data;

f_return:
	if(err)
		delEventServer(out_server);

	return(err);
}
/* Destroys an event server.  'free_user_data', if not null,
 * will be called on the 'user_data'. */
void delEventServer(struct EventServer** server)
{
	if(!server || !*server)return;

	/* Free members. */
	if((*server)->listener)
		evconnlistener_free((*server)->listener);
	if((*server)->signal_event)
		event_free((*server)->signal_event);
	if((*server)->base)
		event_base_free((*server)->base);
	if((*server)->quit_command)
		free((*server)->quit_command);
	if((*server)->free_extended_data )
		(*server)->free_extended_data((*server)->extended_data);

	delArrayList(&(*server)->callback_packages);

	/* Free object. */
	free(*server);
	*server = NULL;
}
/**************************/
