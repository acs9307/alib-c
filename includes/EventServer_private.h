#ifndef EVENT_SERVER_PRIVATE_IS_DEFINED
#define EVENT_SERVER_PRIVATE_IS_DEFINED

#include "EventServer.h"

/*******CLIENT PACKAGE*******/
typedef struct escb_package
{
	EventServer* server;
	void* data;
	alib_free_value free_data_proc;
}escb_package;
/****************************/

/*******EVENT SERVER*******/
/* A single threaded server build on libevent.  This is good for
 * time when a single thread is sufficient for handling requests and
 * is very simple in that no mutexing is required. */
struct EventServer
{
	struct event_base* base;
	struct sockaddr_in sin;
	struct evconnlistener* listener;
	struct event* signal_event;
	char is_running;

	/* Callbacks. */
	es_client_con client_con_cb;
	es_data_in data_in_cb;
	es_client_discon client_discon_cb;
	es_sig sig_cb;

	/* If true, the server will automatically search for a shutdown
	 * command from the clients. */
	char handle_quit_command;
	/* The command to look for to shutdown, default is DEFAULT_QUIT_COMMAND. */
	char* quit_command;
	/* The strlen(quit_command) return value. */
	size_t quit_command_len;

	/* User defined data that can be attached for passing between
	 * callback functions. (OPTIONAL) */
	void* extended_data;
	/* Function used to free the provided user data. (OPTIONAL) */
	alib_free_value free_extended_data;

	/* Holds a list of callback_package objects for later.
	 * Only used to ensure all data is freed. */
	ArrayList* callback_packages;
};
/**************************/
#endif
