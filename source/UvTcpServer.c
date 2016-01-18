#include "UvTcpServer_private.h"

/*******CLIENT PACKAGE*******/
	/* PROTECTED FUNCTIONS */
/* Frees a callback package. */
void free_callback_package(uscb_package* package)
{
	/* Free the user data if possible. */
	if(package->free_data_proc && package->data)
		package->free_data_proc(package->data);

	/* Ensure we have unreserved the memory block. */
	if(package->mem_block)
		MemPool_unreserve_block(&package->mem_block);

	/* Now free the package. */
	free(package);
}
	/***********************/
/****************************/

/*******UV_TCP_SERVER*******/
	/* PRIVATE FUNCTIONS */
		/* STATIC PROTOTYPES */
static void close_client(uv_handle_t* client);
		/*********************/

		/* CALLBACKS */
static void free_wake_handle(uv_handle_t* handle)
{
	free(handle);
}

/* Callback called when SIGINT is received. */
static void signal_interrupt_cb(uv_signal_t* sig_handle, int signum)
{
	if(signum == SIGINT)
	{
		UvTcpServer* server = (UvTcpServer*)sig_handle->data;

		if(server->sigint_cb)
		{
			uscb_rval rval = server->sigint_cb(server, signum);
			if((rval & USCB_SERVER_SHUTDOWN) || (rval & USCB_DEFAULT))
				UvTcpServer_stop(server);
		}
		else
			UvTcpServer_stop(server);
	}
}

/* Callback that should only be called from 'uv_close()'.
 * This will ensure the client is properly removed from the
 * server's client list and that all memory is freed.
 */
static void close_client_cb(uv_handle_t* client)
{
	uscb_package* package;

	/* Check for argument error. */
	if(!client || !client->data)return;

	package = (uscb_package*)client->data;

	/* Call the user's disconnect cb. */
	if(package->server->client_discon_cb)
	{
		uscb_rval rval = package->server->client_discon_cb(package->server, client, package->data);
		if(rval & USCB_CONTINUE)
			return;
		if(rval & USCB_SERVER_SHUTDOWN)
			UvTcpServer_stop(package->server);
	}

	/* Remove the client from the client list. */
	if(package->server)
		ArrayList_remove_no_free_tsafe(package->server->client_list, client);

	/* Free the client and it's data. */
	free_callback_package(package);
	free(client);
}
static void free_client_handle(uv_handle_t* client)
{
    if(client)free(client);
}
/* Removes the client from the list of clients and
 * removes the package from the uscb_package list.
 *
 * Removing the package from the list should free it. */
static void remove_client_cb(void* void_client)
{
	uv_handle_t* client = (uv_handle_t*)void_client;
	uscb_package* package = (uscb_package*)client->data;

	/* Call the user's disconnect cb.
	 * Cannot be continued in this case. */
	if(package->server->client_discon_cb)
	{
		uscb_rval rval = package->server->client_discon_cb(package->server, client, package->data);
		if(rval & USCB_SERVER_SHUTDOWN)
			UvTcpServer_stop(package->server);
	}

	/* Stop reading data from the client. */
	uv_read_stop((uv_stream_t*)client);

	/* Free the client's data. */
	free_callback_package(package);

	/* Now close the client and free it. */
	if(!uv_is_closing((uv_handle_t*)client))
		uv_close(client, free_client_handle);
}
/* Closes the server socket and removes all clients from the
 * server. */
static void close_server_cb(uv_handle_t* server_handle)
{
	UvTcpServer* server = server_handle->data;

	/* Close all clients. */
	ArrayList_clear_tsafe(server->client_list);
	free(server->handle);
	server->handle = NULL;

	/* Stop signal handling. */
	uv_signal_stop(&server->sig_handle);
}

/* Allocates the memory for the data portion of the MemPoolBlock.
 *
 * Of type of 'mem_pool_block_data_alloc_cb'. */
static void input_buff_alloc_cb(void** buff, size_t* size)
{
	if(!buff)return;

	*size = DEFAULT_INPUT_BUFF_SIZE;
	*buff = malloc(*size);
}
/* Frees the input buffer.
 *
 * Of type 'alib_free_value'. */
static void free_input_buff_cb(void* buff)
{
	free(buff);
}

/* Sets the client buffer.  It is a static buffer used by all clients.
 * Cannot be freed!!!
 */
static void client_buff_init(uv_handle_t* client, size_t suggested_size,
		uv_buf_t* buff)
{
	uscb_package* package = (uscb_package*)client->data;

	if(package)
	{
		package->mem_block = MemPool_reserve_block(package->server->input_buffs);

		buff->base = MemPoolBlock_get_data(package->mem_block);
		buff->len = MemPoolBlock_get_data_size(package->mem_block);
	}
}

/* Called when data is received from a client.  If the server checks
 * for quit commands, the user's 'data_in_cb' will be called after
 * checking for a quit command.
 *
 * Parameters:
 * 		 client: The handle to the client.  The client's data will contain
 * 		 	the callback package for the client.
 * 		 nread: The number of bytes read from the client.  If this is a negative
 * 		 	number, it signifies an error occurred.  If this happens, the
 * 		 	client will be closed and the 'data_in_cb' will not be called.
 * 		 buf: The 'uv_buf_t' struct that holds the input buffer.
 */
static void client_data_in_cb(uv_stream_t* client, ssize_t nread, const uv_buf_t* buf)
{
	uscb_package* package = (uscb_package*)client->data;
	UvTcpServer* server = (UvTcpServer*)package->server;

	/* Error occurred, we need to close the client. */
	if(nread < 0)
	{
		ArrayList_remove_tsafe(server->client_list, client);
		goto f_return;
	}
	/* Check if a quit command was received. */
	if(server->handle_quit_command && str_match(buf->base, nread,
			(server->quit_command)?server->quit_command:DEFAULT_QUIT_COMMAND,
			0) == 0)
	{
		if(server->qc_cb)
		{
			uscb_rval rval = server->qc_cb(server, client);
			if(rval & USCB_CLIENT_CLOSE)
				ArrayList_remove_tsafe(server->client_list, client);
			if(rval & USCB_CONTINUE)
				goto f_return;
		}
		UvTcpServer_stop(server);
	}
	/* Call the user's data_in_cb. */
	else if(server->data_in_cb)
	{
		uscb_rval r_val;

		r_val = server->data_in_cb(server, client, buf->base, nread, package->data);
		if((r_val & USCB_CLIENT_CLOSE) || (r_val & USCB_SERVER_SHUTDOWN))
		{
			ArrayList_remove_tsafe(server->client_list, client);
			if(r_val & USCB_SERVER_SHUTDOWN)
				UvTcpServer_stop(server);
			goto f_return;
		}
	}

f_return:
	/* Need to unreserve the memory block. */
	if(package)
		MemPool_unreserve_block(&package->mem_block);
}

/* Called when a client connects to the server.
 *
 * Parameters:
 * 		server_handle: A handle to the server that a client
 * 			connected to.
 * 		status: Should be 0.  Don't know why it would be any
 * 			other value.  Documentation says <0 is error.
 */
static void client_connection_cb(uv_stream_t* server_handle, int status)
{
	UvTcpServer* server = (UvTcpServer*)server_handle->data;
	uv_tcp_t* client_handle;
	uscb_package* package;

	/* Allocate needed memory. */
	client_handle = malloc(sizeof(uv_tcp_t));
	if(!client_handle)return;
	package = malloc(sizeof(uscb_package));
	if(!package)
	{
		free(client_handle);
		return;
	}

	/* Initialize and accept the client. */
	if(uv_tcp_init(server_handle->loop, client_handle) ||
	   uv_accept(server_handle, (uv_stream_t*)client_handle))
	{
		free(client_handle);
		free(package);
		return;
	}

	/* Initialize the package. */
	package->server = server;
	package->data = server;
	package->free_data_proc = NULL;
	package->mem_block = NULL;

	/* Call the client's connection callback. */
	if(server->client_con_cb)
	{
		/* Call the client connection callback. */
		uscb_rval r_code = server->client_con_cb(server, client_handle, &package->data,
				&package->free_data_proc);

		/* Check to see if we need to close anything down. */
		if((r_code & USCB_CLIENT_CLOSE) || (r_code & USCB_SERVER_SHUTDOWN))
		{
			close_client((uv_handle_t*)client_handle);
			free(client_handle);
			free(package);

			if(r_code & USCB_SERVER_SHUTDOWN)
				UvTcpServer_stop(server);
			return;
		}
	}

	/* Add the package */
	client_handle->data = package;

	/* Add the client to the client list. */
	ArrayList_add_tsafe(server->client_list, client_handle);

	/* Start listening for data sent by the client. */
	if(uv_read_start((uv_stream_t*)client_handle, client_buff_init, client_data_in_cb))
		ArrayList_remove_tsafe(server->client_list, client_handle);
}
		/*************/

/* Closes a specific client.  This usually should not be called outside the
 * object, but is provided for convenience.  Also required for ArrayList to
 * automatically close a client when it is removed from the list.
 *
 * Parameters:
 * 		client: A 'uv_handle_t*' object to the client.
 */
static void close_client(uv_handle_t* client)
{
	if(client && !uv_is_closing(client))
		uv_close(client, close_client_cb);
}
	/*********************/

	/* PUBLIC FUNCTIONS */
/* Starts a UvTcpServer running.  This function will block
 * the calling thread until the server is shutdown.
 *
 * Returns:
 * 		alib_error: Error occurred.
 * 		Anything else: Return code of 'uv_run()'.
 */
int UvTcpServer_start(UvTcpServer* server)
{
	int rcode;

	/* Check for bad arg. */
	if(!server)return(ALIB_BAD_ARG);

	/* Ensure the server is stopped before going on. */
	UvTcpServer_stop(server);

	/* Allocate the memory for the handle if need be. */
	if(!(server->handle = malloc(sizeof(uv_tcp_t))))
	{
		rcode = ALIB_MEM_ERR;
		goto f_error;
	}

	/* Initialize the handle. */
	if(uv_tcp_init(server->loop, server->handle))
	{
		rcode = ALIB_UNKNOWN_ERR;
		goto f_error;
	}

	/* Bind the port.  If this fails, most likely means
	 * port is already in use. */
	if(uv_tcp_bind(server->handle, (struct sockaddr*)&server->addr, 0))
	{
		rcode = ALIB_PORT_IN_USE;
		goto f_error;
	}

	/* Sets up the server to start listening. */
	server->handle->data = server;
	if(uv_listen((uv_stream_t*)server->handle, server->backlog_size, client_connection_cb))
	{
		rcode = ALIB_UNKNOWN_ERR;
		goto f_error;
	}

	/* Start listening for the signal interrupt. */
	if(uv_signal_start(&server->sig_handle, signal_interrupt_cb, SIGINT))
	{
		rcode = ALIB_UNKNOWN_ERR;
		goto f_error;
	}
	server->sig_handle.data = server;

	return(uv_run(server->loop, UV_RUN_DEFAULT));
f_error:
	/* Free resources as needed. */
	if(server->handle)
	{
		free(server->handle);
		server->handle = NULL;
	}

	return(rcode);
}
/* Stops the server.
 *
 * Parameters:
 * 		server: The UvTcpServer to stop.
 */
void UvTcpServer_stop(UvTcpServer* server)
{
	uv_async_t* wake_handle;
	if(server && server->handle && !uv_is_closing((uv_handle_t*)server->handle))
		uv_close((uv_handle_t*)server->handle, close_server_cb);

	wake_handle = malloc(sizeof(uv_async_t));
	if(wake_handle)
	{
		uv_signal_stop(&server->sig_handle);

		if(uv_async_init(server->loop, wake_handle, NULL) || uv_async_send(wake_handle))
			free(wake_handle);
		else
			uv_close((uv_handle_t*)wake_handle, free_wake_handle);
	}
}

/* Adds a client from an already open socket. The server's connection_cb
 * will be called on the client after it has been added.
 *
 * Parameters:
 * 		server: The UvTcpServer that the client should be added to.
 * 		sock: The socket of the client that will be added to the server.
 */
int UvTcpServer_add_client(UvTcpServer* server, int sock)
{
	uv_tcp_t* client_handle;
	uscb_package* package;
	int err;

	if(!server || sock < 0)return(ALIB_BAD_ARG);

	/* Allocate needed memory. */
	client_handle = malloc(sizeof(uv_tcp_t));
	if(!client_handle)return(ALIB_MEM_ERR);
	package = malloc(sizeof(uscb_package));
	if(!package)
	{
		err = ALIB_MEM_ERR;
		goto f_error;
	}

	/* Initialize the client handle. */
	if(uv_tcp_init(server->loop, client_handle) ||
			uv_tcp_open(client_handle, sock))
	{
		err = ALIB_FD_ERROR;
		goto f_error;
	}

	/* Initialize the package. */
	package->server = server;
	package->data = server;
	package->free_data_proc = NULL;
	package->mem_block = NULL;

	/* Call the client's connection callback. */
	if(server->client_con_cb)
	{
		/* Call the client connection callback. */
		uscb_rval r_code = server->client_con_cb(server, client_handle, &package->data,
				&package->free_data_proc);

		/* Check to see if we need to close anything down. */
		if((r_code & USCB_CLIENT_CLOSE) || (r_code & USCB_SERVER_SHUTDOWN))
		{
			close_client((uv_handle_t*)client_handle);

			err = ALIB_OK;
			if(r_code & USCB_SERVER_SHUTDOWN)
				UvTcpServer_stop(server);

			goto f_error;
		}
	}

	/* Add the package */
	client_handle->data = package;

	/* Add the client to the client list. */
	ArrayList_add_tsafe(server->client_list, client_handle);

	/* Start listening for data sent by the client. */
	if(uv_read_start((uv_stream_t*)client_handle, client_buff_init, client_data_in_cb))
		ArrayList_remove_tsafe(server->client_list, client_handle);
	uv_run(client_handle->loop, UV_RUN_NOWAIT);

	return(ALIB_OK);
f_error:
	if(client_handle)free(client_handle);
	if(package)free(package);

	return(err);
}
/* Removes a client from the server and closes the client's socket.
 *
 * This is safe to call within the callback functions, however it
 * is still suggested to use the USCB_CLIENT_CLOSE return code instead.
 *
 * Parameters:
 * 		server: The server the client belongs to.
 * 		client: The client to remove from the server.
 */
void UvTcpServer_remove_client(UvTcpServer* server, uv_handle_t* client)
{
	if(server && client)
		ArrayList_remove(server->client_list, client);
}

		/* Setters */
/* Sets the server's client_con_cb to the given callback.
 *
 * Assumes server is not null. */
void UvTcpServer_set_client_con_cb(UvTcpServer* server, us_client_con client_con_cb)
{
	server->client_con_cb = client_con_cb;
}
/* Sets the server's data_in_cb to the given callback.
 *
 * Assumes server is not null. */
void UvTcpServer_set_data_in_cb(UvTcpServer* server, us_data_in data_in_cb)
{
	server->data_in_cb = data_in_cb;
}
/* Sets the server's client_discon_cb to the given callback.
 *
 * Assumes server is not null. */
void UvTcpServer_set_client_discon_cb(UvTcpServer* server,
		us_client_discon client_discon_cb)
{
	server->client_discon_cb = client_discon_cb;
}
/* Sets the server's sigint callback.
 *
 * Assumes server is not null. */
void UvTcpServer_set_sigint_cb(UvTcpServer* server,
		us_sigint_cb sigint_cb)
{
	server->sigint_cb = sigint_cb;
}
void UvTcpServer_set_quit_command_cb(UvTcpServer* server,
		us_quit_command_cb qc_cb)
{
	server->qc_cb = qc_cb;
}

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
		const char* quit_cmd)
{
	if(!server)return;

	/* Ensure we free the old memory before reallocating. */
	if(server->quit_command)
	{
		free(server->quit_command);
		server->quit_command = NULL;
	}

	server->handle_quit_command = handle_quit_cmd;
	/* If we need to allocate memory, allocate it and copy the data over. */
	if(handle_quit_cmd)
	{
		if(quit_cmd)
		{
			server->quit_command_len = strlen(quit_cmd);
			strncpy_alloc(&server->quit_command, quit_cmd, server->quit_command_len);
		}
		else
			server->quit_command_len = strlen(DEFAULT_QUIT_COMMAND);
	}
}
/* Sets the backlog size of the server.  If the server is already running,
 * it will change the value then restart the server. */
void UvTcpServer_set_backlog_size(UvTcpServer* server, size_t backlog_size)
{
	if(!server)return;

	server->backlog_size = backlog_size;

	if(UvTcpServer_is_running(server))
	{
		UvTcpServer_stop(server);
		UvTcpServer_start(server);
	}
}
/* Sets the maximum number of input buffers allowed to be allocated at one
 * time.  By default, this is set to DEFAULT_BACKLOG_SIZE.
 *
 * Assumes server is not null.
 */
void UvTcpServer_set_max_input_buffer_cap(UvTcpServer* server, size_t max_size)
{
	ArrayList_set_max_cap((ArrayList*)server->input_buffs, max_size);
}

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
		alib_free_value free_extended_data)
{
	/* Free old data if we can. */
	if(server->free_extended_data && server->extended_data)
		server->free_extended_data(server->extended_data);

	/* Set the new values. */
	server->extended_data = extended_data;
	server->free_extended_data = free_extended_data;
}
		/***********/

		/* Getters */
/* Returns !0 if the server is running, 0 otherwise.
 *
 * Assumes server is not null. */
char UvTcpServer_is_running(UvTcpServer* server)
{
	return((server->handle != NULL));
}

/* Gets the backlog size of the server.
 *
 * Assumes the server is not null. */
size_t UvTcpServer_get_backlog_size(UvTcpServer* server)
{
	return(server->backlog_size);
}
/* Gets the extended data of the server.
 *
 * Assumes the server is not null. */
void* UvTcpServer_get_extended_data(UvTcpServer* server)
{
	return(server->extended_data);
}

/* Returns the number of clients connected to the server.
 *
 * Assumes 'server' is not null. */
size_t UvTcpServer_get_client_count(UvTcpServer* server)
{
	return(ArrayList_get_count(server->client_list));
}
/* Returns !0 if the server will handle quit commands,
 * 0 otherwise.
 *
 * Assumes 'server' is not null.
 */
char UvTcpServer_will_handle_quit(UvTcpServer* server)
{
	return(server->handle_quit_command);
}
/* Returns the quit command that will be handled by the server if
 * the handle quit command flag is raised.
 *
 * Assumes 'server' is not null. */
const char* UvTcpServer_get_quit_command(UvTcpServer* server)
{
	if(server->quit_command)
		return(server->quit_command);
	else
		return(DEFAULT_QUIT_COMMAND);
}

/* Returns the number of input buffers that have been allocated.
 *
 * Assumes server is not null. */
size_t UvTcpServer_get_input_buff_count(UvTcpServer* server)
{
	return(ArrayList_get_count((ArrayList*)server->input_buffs));
}

/* Returns the address struct for the server.
 *
 * Assumes server is not null. */
const struct sockaddr_in* UvTcpServer_get_addr(UvTcpServer* server)
{
	return(&server->addr);
}

/* Returns the loop of the UvTcpServer.
 *
 * Assumes server is not null.
 */
const uv_loop_t* UvTcpServer_get_loop(UvTcpServer* server)
{
	return(server->loop);
}
		/***********/

		/* Constructors */
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
UvTcpServer* newUvTcpServer(uint16_t port, char handle_quit_command)
{
	UvTcpServer* server = malloc(sizeof(UvTcpServer));
	if(!server)return(NULL);

	/* Initialize server behavior members. */
	server->handle = NULL;
	server->loop = uv_default_loop();

	/* Initialize server socket members. */
	memset(&server->addr, 0, sizeof(server->addr));
	server->addr.sin_family = AF_INET;
	server->addr.sin_port = htons(port);
	server->backlog_size = DEFAULT_BACKLOG_SIZE;

	/* Initialize the list of clients and input
	 * buffers. */
	server->client_list = newArrayList(remove_client_cb);
	server->input_buffs = newMemPool(input_buff_alloc_cb, free_input_buff_cb);

	/* Initialize callbacks. */
	server->client_con_cb = NULL;
	server->data_in_cb = NULL;
	server->client_discon_cb = NULL;
	server->sigint_cb = NULL;
	server->qc_cb = NULL;

	/* Initialize other members. */
	server->handle_quit_command = handle_quit_command;
	server->quit_command = NULL;
	server->extended_data = NULL;
	server->free_extended_data = NULL;
	server->quit_command_len = strlen(DEFAULT_QUIT_COMMAND);

	/* Ensure everything was initialized correctly. */
	if(!server->loop || !server->client_list || !server->input_buffs ||
			uv_signal_init(server->loop, &server->sig_handle))
		delUvTcpServer(&server);

	return(server);
}
/* Deletes the server and sets the pointer to NULL.
 * If the server is running, it will be shutdown.
 *
 * Parameters:
 * 		server: UvTcpServer to destroy.  If successfully
 * 			destroyed, the pointer will be set to NULL.
 */
void delUvTcpServer(UvTcpServer** server)
{
	/* Check for errors. */
	if(!server || !*server)return;

	/* Stop the server if it is running. */
	UvTcpServer_stop(*server);
	/* Delete and close all clients. */
	delArrayList(&(*server)->client_list);

	/* Free the server and set it NULL. */
	free(*server);
	*server = NULL;
}
		/****************/
	/********************/
/***************************/
