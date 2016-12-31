#include "includes/UvTcpClient_private.h"

/*******Private Structs*******/
typedef struct run_pack
{
	UvTcpClient* client;
	int mode;
}run_pack;
/*****************************/

/*******Private Functions*******/
static void alloc_cb(uv_handle_t* client_handle, size_t suggested_size, uv_buf_t* buf)
{
	UvTcpClient* client = (UvTcpClient*)client_handle->data;

	/* Set the uv buffer. */
	buf->base = client->recv_buff;
	buf->len = client->recv_buff_size;
}
static void read_cb(uv_stream_t* client_handle, ssize_t nread, const uv_buf_t* buf)
{
	UvTcpClient* client = (UvTcpClient*)client_handle->data;

	/* Check to see if the client was disconnected by the host. */
	if(nread < 0)
	{
		UvTcpClient_disconnect(client);
		return;
	}

	/* Call the user's read callback. */
	if(client->client_read)
		client->client_read(client, buf->base, nread);
}

/* Called whenever the client is closed or disconnected from the host.
 * Wrapper function to call the user specified 'client_close' callback.
 * Solely used to safely extract the client object from the handle and
 * pass it to the user's callback. */
static void close_cb(uv_handle_t* client_handle)
{
	UvTcpClient* client = client_handle->data;

	/* Call the user defined callback. */
	if(client->client_close)
		client->client_close(client);

	/* Set members as needed. */
	client->connected = 0;
	if(client->recv_buff)
	{
		free(client->recv_buff);
		client->recv_buff = NULL;
	}
}

/* Called whenever a client connects to the host.
 * This is just a wrapper function for calling the client's
 * client_connect callback.  This is designed this way so that
 * the actual client object can be passed to the callback rather
 * than leaving the user to find the client data. */
static void connected_cb(uv_connect_t* con_handle, int status)
{
	UvTcpClient* client = (UvTcpClient*)con_handle->data;

	/* Call the connection callback. */
	if(client->client_connect)
		client->client_connect(client, status);

	if(status)
		client->connected = 0;
	else
	{
		/* Allocate memory for buff if needed. */
		if(!client->recv_buff)
			client->recv_buff = malloc(client->recv_buff_size);

		/* Start the reading on the client. */
		uv_read_start((uv_stream_t*)&client->handle, alloc_cb, read_cb);
	}
}

	/* Thread Procs */
/* Runs the client loop when connected using threading. */
static void* run_loop_proc(void* void_run_pack)
{
	run_pack* pack = (run_pack*)void_run_pack;

	pack->client->thread_running = 1;
	uv_run(pack->client->handle.loop, pack->mode);
	pack->client->thread_running = 0;

	if(pack)free(pack);
	return(NULL);
}
	/****************/
/*******************************/

/*******Public Functions*******/
/* Connects the client to its specified host.  This function will block until
 * the client disconnects.  For a non-blocking solution, use
 * UvTcpClient_connect_threaded().
 *
 * Parameters:
 * 		client: The client to connect to its host.
 */
alib_error UvTcpClient_connect(UvTcpClient* client)
{
	int err_code;

	/* Ensure the client is not null. */
	if(!client)
		return(ALIB_BAD_ARG);

	/* Ensures the client is first disconnected before trying
	 * to connect. */
	if(client->connected)
		UvTcpClient_disconnect(client);

	/* Initialize the TCP handler. */
	if((err_code = uv_tcp_init(uv_default_loop(), &client->handle)))
		return(err_code);

	/* Open a new socket. */
	client->sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if(client->sock < 0)return(ALIB_FD_ERR);
	err_code = uv_tcp_open(&client->handle, client->sock);
	if(err_code)return(ALIB_TCP_CONNECT_ERR);

	/* Connect to the client. */
	if((err_code = uv_tcp_connect(&client->con_handle, &client->handle,
			(struct sockaddr*)&client->addr, connected_cb)))
		return(err_code);

	/* Setup members. */
	client->handle.data = client;
	client->con_handle.data = client;
	client->connected = 1;

	/* Start the loop. */
	uv_run(client->handle.loop, UV_RUN_DEFAULT);
	return(ALIB_OK);
}
/* Connects the client its specified host using a separate thread,
 * therefore this function will not block.  To stop the thread,
 * simple disconnect the client.
 *
 * Parameters:
 * 		client: The client to connect to its host.
 */
alib_error UvTcpClient_connect_threaded(UvTcpClient* client)
{
	int err_code;
	run_pack* pack;

	/* Ensure the client is not null. */
	if(!client)
		return(ALIB_BAD_ARG);

	/* Ensures the client is first disconnected before trying
	 * to connect. */
	if(client->connected)
		UvTcpClient_disconnect(client);

	/* Initialize the TCP handler. */
	if((err_code = uv_tcp_init(uv_default_loop(), &client->handle)))
		return(err_code);

	/* Open a new socket. */
	client->sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if(client->sock < 0)return(ALIB_FD_ERR);
	err_code = uv_tcp_open(&client->handle, client->sock);
	if(err_code)return(ALIB_TCP_CONNECT_ERR);

	/* Connect to the client. */
	if((err_code = uv_tcp_connect(&client->con_handle, &client->handle,
			(struct sockaddr*)&client->addr, connected_cb)))
		return(err_code);

	/* Setup members. */
	client->handle.data = client;
	client->con_handle.data = client;
	client->connected = 1;

	/* Start the loop. */
	pack = malloc(sizeof(run_pack));
	if(!pack)return(ALIB_MEM_ERR);
	pack->client = client;
	pack->mode = UV_RUN_DEFAULT;
	pthread_create(&client->thread, NULL, run_loop_proc, pack);

	return(ALIB_OK);
}

/* Disconnects the client from the host.
 *
 * Parameters:
 * 		client: The client object to be disconnected. */
void UvTcpClient_disconnect(UvTcpClient* client)
{
	if(client && client->connected)
	{
		if(!uv_is_closing((uv_handle_t*)&client->handle))
			uv_close((uv_handle_t*)&client->handle,
				close_cb);

		/* Cleanup the thread. */
		pthread_join(client->thread, NULL);
	}
}

	/* Getters */
/* Returns !0 if connected and 0 if not connected.
 *
 * Assumes client is not null. */
char UvTcpClient_is_connected(UvTcpClient* client)
{
	return(client->connected);
}
/* Returns the size of the receive buffer for the client.
 *
 * Assumes 'client' is not null. */
size_t UvTcpClient_get_recv_buff_size(UvTcpClient* client)
{
	return(client->recv_buff_size);
}
/* Returns the client's handle if the client is connected, NULL otherwise.
 *
 * Assumes 'client' is not null. */
const uv_tcp_t* UvTcpClient_get_handle(UvTcpClient* client)
{
	if(client->connected)
		return(&client->handle);
	else
		return(NULL);
}
/* Returns the socket for the client.
 *
 * Assumes 'client' is not null. */
int UvTcpClient_get_sock(UvTcpClient* client)
{
	return(client->sock);
}

/* Returns 1 if the thread is running, 0 if it is not.
 * If UvTcpClient_connect_threaded(), the loop may still be
 * running, but only on the calling thread.  To ensure,
 * check 'UvTcpClient_is_connected()' to see if the
 * client loop is still running.
 *
 * Assumes 'client' is not null. */
char UvTcpClient_thread_is_running(UvTcpClient* client)
{
	return(client->thread_running);
}
	/***********/

	/* Setters */
/* Sets the client connect callback for the UvTcpClient.
 *
 * Assumes 'client' is not null. */
void UvTcpClient_set_client_connect_cb(UvTcpClient* client,
		uc_client_connect_cb client_connect_cb)
{
	client->client_connect = client_connect_cb;
}
/* Sets the client disconnect callback for the UvTcpClient.
 *
 * Assumes 'client' is not null. */
void UvTcpClient_set_client_disconnect_cb(UvTcpClient* client,
		uc_client_close_cb client_disconnect_cb)
{
	client->client_close = client_disconnect_cb;
}
/* Sets the client read callback for the UvTcpClient. */
void UvTcpClient_set_client_read_cb(UvTcpClient* client,
		uc_client_read_cb client_read_cb)
{
	if(!client)return;

	client->client_read = client_read_cb;
}

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
void UvTcpClient_set_recv_buff_size(UvTcpClient* client, size_t size)
{
	if(client && (!client->connected || !client->recv_buff))
	{
		if(size == 0)size = 1;

		client->recv_buff_size = size;

		if(client->recv_buff)
			client->recv_buff = realloc(client->recv_buff, size);
	}
}
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
UvTcpClient* newUvTcpClient(const char* addr, uint16_t port)
{
	UvTcpClient* client;

	/* Check for bad arguments. */
	if(!addr)return(NULL);

	/* Allocate the client. */
	client = malloc(sizeof(UvTcpClient));
	if(!client)return(NULL);

	/* Set the ipv4 address. */
	uv_ip4_addr(addr, port, &client->addr);

	/* Initialize all other members. */
	client->connected = 0;
	client->recv_buff = NULL;
	client->recv_buff_size = DEFAULT_INPUT_BUFF_SIZE;
	client->thread_running = 0;

	return(client);
}
/* Deletes a UvTcpClient object and sets the pointer to NULL.
 * If the client is already connect, the client will first be disconnected before
 * freeing memory. */
void delUvTcpClient(UvTcpClient** client)
{
	if(!client || !*client)return;

	/* We must ensure we are disoconnected first. */
	UvTcpClient_disconnect(*client);

	if((*client)->recv_buff)
		free((*client)->recv_buff);

	free(*client);
	*client = NULL;
}
/**************************/
