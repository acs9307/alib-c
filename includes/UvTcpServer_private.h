#ifndef UV_TCP_SERVER_PRIVATE_IS_DEFINED
#define UV_TCP_SERVER_PRIVATE_IS_DEFINED

#include "UvTcpServer.h"

/* A package that is attached to uv_handle_t structs.
 * It contains server data, memory block data and user data.
 * Handled internally only, and should not be seen by the user. */
typedef struct uscb_package
{
	UvTcpServer* server;
	void* data;
	alib_free_value free_data_proc;

	MemPoolBlock* mem_block;
}uscb_package;

/* A wrapper object for creating a libuv tcp server.
 * This is a multi-threaded server that handles request by callbacks.
 * Though all callbacks have default behavior, users can modify
 * the behavior by handling the different callbacks. */
struct UvTcpServer
{
	/* The tcp handle object for the server.
	 * This is actually passed into the callbacks, but
	 * the server will be set as it's data member.  It is
	 * a nasty work-around for libuv.
	 *
	 * When NULL, the handle has not been initialized.  Should
	 * not be initialized until server is started. */
	uv_tcp_t* handle;
	/* Loop for the object. */
	uv_loop_t* loop;
	/* The socket meta data for the server. */
	struct sockaddr_in addr;
	/* Max number of clients that can wait to be accepted. */
	size_t backlog_size;
	/* Stores 'uv_tcp_t' objects, one for each client.
	 * Removed when the client disconnects. */
	ArrayList* client_list;
	/* Buffer pool that stores all the buffers from the */
	MemPool* input_buffs;

	/* Signal handling. */
	uv_signal_t sig_handle;

	/* Callbacks. */
	us_client_con client_con_cb;
	us_data_in data_in_cb;
	us_client_discon client_discon_cb;
	us_sigint_cb sigint_cb;
	us_quit_command_cb qc_cb;

	/* If true, the server will automatically search for a shutdown
	 * command from the clients. */
	char handle_quit_command;
	/* The command to look for to shutdown, default is DEFAULT_QUIT_COMMAND. */
	char* quit_command;
	/* strlen(quit_command). */
	size_t quit_command_len;

	/* User defined data that can be attached for passing between
	 * callback functions. (OPTIONAL) */
	void* extended_data;
	/* Function used to free the provided user data. (OPTIONAL) */
	alib_free_value free_extended_data;
};

#endif
