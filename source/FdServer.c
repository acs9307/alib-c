#include "FdServer_private.h"

/**************fds_package**************/
	/* Private Functions */
static fds_package* new_fds_package(int sock)
{
	fds_package* package = malloc(sizeof(fds_package));
	if(!package)return(NULL);

	*((int*)&package->sock) = sock;
	package->user_data = NULL;
	package->free_user_data = NULL;

	return(package);
}
static void free_fds_package(fds_package* package)
{
	if(!package)return;

	if(package->sock > -1)
		close(package->sock);

	if(package->user_data && package->free_user_data)
		package->free_user_data(package->user_data);
	free(package);
}
	/*********************/
/***************************************/


/***************FdServer***************/
	/*******Private Functions*******/

		/* Private Threaded Functions */
/* Used for freeing clients from the ArrayList. */
static void free_client(void* void_sock)
{
	fds_package* pack = (fds_package*)void_sock;
	free_fds_package(pack);
}

/* Thread function that will run the server.
 * The run_package will be freed upon return. */
static void* start_thread(void* v_server)
{
	FdServer* server = (FdServer*)v_server;

	flag_raise(&server->flag_pole, THREAD_IS_RUNNING);
	run_loop(server);
	flag_lower(&server->flag_pole, THREAD_IS_RUNNING);

	return(NULL);
}
		/******************************/

/* Initializes all the sockets and what-not so that we can start listening. */
static alib_error run_init(FdServer* server)
{
	alib_error rval = 0;

	if(!server)return(ALIB_BAD_ARG);

	/* Setup the socket. */
	if(server->sock < 0)
	{
		rval = FdServer_bind(server);
		if(rval)return(rval);
	}
	rval = set_sock_non_block(server->sock);
	if(rval)return(rval);

	/* Listen on the socket. */
	rval = listen(server->sock, DEFAULT_BACKLOG_SIZE);
	if(rval)
		goto f_error;

	/* Setup the epoll object. */
	server->ep.efd = epoll_create(DEFAULT_BACKLOG_SIZE);
	if(server->ep.efd < 0)
	{
		rval = ALIB_FD_ERR;
		goto f_error;
	}
	server->ep.event.data.fd = server->sock;
	server->ep.event.events = EPOLLIN;
	rval = epoll_ctl(server->ep.efd, EPOLL_CTL_ADD, server->sock, &server->ep.event);
	if(rval < 0)
	{
		rval = ALIB_UNKNOWN_ERR;
		goto f_error;
	}
	memset(server->ep.events, 0, sizeof(server->ep.events));

	return(rval);
f_error:
	if(server->sock > -1)
	{
		close(server->sock);
		server->sock = -1;
	}
	if(server->ep.efd > -1)
	{
		close(server->ep.efd);
		server->ep.efd = -1;
	}
	return(rval);
}

/* The main loop for listening. */
static alib_error run_loop(FdServer* server)
{
	if(!server)return(ALIB_BAD_ARG);

	int rval;
	int event_count;
	struct epoll_event* event_it;

	/* Start listen and accept loop. */
	while(server->sock > -1 && server->ep.efd > -1)
	{
		/* Wait for an event. Check every second to ensure the server's socket
		 * has not been closed. */
		event_count = epoll_wait(server->ep.efd, server->ep.events,
				DEFAULT_BACKLOG_SIZE, 1000);

		if(!event_count)continue;

		/* An error occurred, return. */
		if(event_count < 0)
		{
			if(server->flag_pole & THREAD_STOP)
				rval = ALIB_OK;
			else
			{
				if(server->sock > -1 && server->ep.efd > -1)
					continue;
				else
					rval = ALIB_CHECK_ERRNO;
			}
			goto f_return;
		}

		/* We got a message, check all the sockets for data. */
		for(event_it = server->ep.events; event_count > 0; ++event_it, --event_count)
		{
			/* Server socket received a client. */
			if(event_it->data.fd == server->sock)
			{
				int new_sock;
				fds_package* new_client;

				if((event_it->events & (EPOLLERR | EPOLLHUP)) ||
						!(event_it->events & EPOLLIN))
				{
					FdServer_stop(server);
					rval = ALIB_CHECK_ERRNO;
					goto f_return;
				}
				new_sock = accept(server->sock, NULL, NULL);

				/* Ensure the socket was opened correctly. */
				if(new_sock < 0)continue;

				/* Create a new package object. */
				new_client = new_fds_package(new_sock);
				if(!new_client)
				{
					close(new_sock);
					rval = ALIB_MEM_ERR;
					goto f_return;
				}

				/* Call the user's on connect callback. */
				if(server->on_connect)
				{
					rval = server->on_connect(server, new_client);
					if((rval & SCB_RVAL_CLOSE_CLIENT) ||
							(rval & SCB_RVAL_STOP_SERVER))
					{
						free_fds_package(new_client);
						new_client = NULL;
					}
					if(rval & SCB_RVAL_STOP_SERVER)
					{
						rval = ALIB_OK;
						goto f_return;
					}

					/* If the client was closed, then we just need to
					 * continue. */
					if(!new_client)
						continue;
				}

				/* The client was accepted, now add it to the list. */
				server->ep.event.data.fd = new_client->sock;
				server->ep.event.events = EPOLLIN;
				rval = epoll_ctl(server->ep.efd, EPOLL_CTL_ADD, new_client->sock,
						&server->ep.event);
				if(rval < 0)
				{
					free_fds_package(new_client);
					continue;
				}
				/* Add the client to the array list. */
				if(!ArrayList_add(server->clients, new_client))
				{
					free_fds_package(new_client);
					rval = ALIB_MEM_ERR;
					goto f_return;
				}
			}
			/* Client socket received data. */
			else
			{
				int new_fd;
				fds_package* package = (fds_package*)ArrayList_find_item_by_value_tsafe
						(server->clients, &server->ep.events->data.fd, compare_int_ptr);
				if(!package)
				{
					close(server->ep.events->data.fd);
					continue;
				}

				/* Error occurred on the socket, most likely disconnected. */
				if((event_it->events & (EPOLLERR | EPOLLHUP)) ||
						!(event_it->events & EPOLLIN))
				{
					FdServer_close_client(server, package);
					continue;
				}

				/* Read the data from the file descriptor. */
				if(ancil_recv_fd(server->ep.events->data.fd, &new_fd) == 0 &&
						server->on_receive && package)
				{
					rval = server->on_receive(server, package,new_fd);
					if((rval & SCB_RVAL_CLOSE_CLIENT) || (rval & SCB_RVAL_STOP_SERVER))
						FdServer_close_client(server, package);
					if(rval & SCB_RVAL_STOP_SERVER)
					{
						rval = ALIB_OK;
						goto f_return;
					}
				}
				/* Read failed! */
				else
				{
					FdServer_close_client(server, package);
					continue;
				}
			}
		}
	}
	rval = ALIB_OK;

f_return:
	/* Ensure our socket has been properly closed. */
	FdServer_stop(server);

	return(rval);
}
	/*******************************/

	/*******Public Functions*******/
/* Closes the given client that is connected to the server.
 *
 * If the package does not belong to the given server, the client
 * will not be closed.
 *
 * Parameters:
 * 		server: The server object that is related to 'sock'.
 * 		package: The client package belonging to the server.
 */
void FdServer_close_client(FdServer* server, fds_package* package)
{
	if(!server)return;

	/* Call the user's on_disconnect callback. */
	if(server->on_disconnect)
	{
		server_cb_rval rval = server->on_disconnect(server, package);
		if(rval & SCB_RVAL_STOP_SERVER)
			FdServer_stop(server);
	}
	ArrayList_remove_tsafe(server->clients, package);
}
/* Closes a client connected to the FdServer based on the given socket.
 * This should be called rather than 'close()' as it frees memory within
 * the FdServer.
 *
 * The client will be closed even if it is not found in the server,
 * it is highly suggested to use FdServer_close_client() instead.
 *
 * This is designed for when you don't have the fds_package for the client
 * or you are just not sure where the socket came from.
 *
 * Parameters:
 * 		server: The server object that is related to 'sock'.
 * 		sock: The socket of the client that should be closed.
 */
void FdServer_close_client_by_socket(FdServer* server, int sock)
{
	fds_package* package;

	if(!server)return;

	package = (fds_package*)ArrayList_find_item_by_value_tsafe(server->clients, &sock, compare_int_ptr);
	if(package == NULL)
	{
		close(sock);
		return;
	}
	else
		FdServer_close_client(server, package);
}

/* Allocates then binds the socket.
 *
 * Returns:
 * 		ALIB_FD_ERROR: Error occurred during creation or binding of socket.
 *
 * Note:
 * 		If an error occurs, the FdServer's socket is guaranteed to be set to -1.
 */
alib_error FdServer_bind(FdServer* server)
{
	int err;
	int optval = 1;

	/* Check for null args. */
	if(!server)return(ALIB_BAD_ARG);

	/* Ensure the socket is closed. */
	close(server->sock);
	close(server->ep.efd);
	server->sock = -1;
	server->ep.efd = -1;

	/* Create the socket. */
	server->sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if(server->sock < 0)goto f_error;
	setsockopt(server->sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

	/* Bind the socket. */
	err = bind(server->sock, (struct sockaddr*)&server->addr,
			sizeof(server->addr));
	if(err)goto f_error;

	return(ALIB_OK);

	/* If an error occurred, make sure 'sock' is set to -1. */
f_error:
	server->sock = -1;

	return(ALIB_FD_ERR);
}

/* Stops the FdServer.  This will not join the thread if async listening was used.
 *
 * To cleanup the thread after async listening was used, you must call
 * 'FdServer_stop_with_join()' or 'delFdServer()'. */
void FdServer_stop(FdServer* server)
{
	if(server)
	{
		flag_raise(&server->flag_pole, THREAD_STOP);

		if(server->sock > -1)
		{
			close(server->sock);
			server->sock = -1;
		}
		if(server->ep.efd > -1)
		{
			close(server->ep.efd);
			server->ep.efd = -1;
		}

		ArrayList_clear_tsafe(server->clients);
	}
}
/* Stops the FdServer.  If FdServer_run_on_thread() is used, this should not be
 * called within the listening thread, that includes the callback functions,
 * as the thread will attempt to join itself. If you must stop within the listening
 * loop, call 'FdServer_stop()' or, better yet, return the SCB_RVAL_STOP_SERVER.
 *
 * This function WILL BLOCK until the listening thread returns. */
void FdServer_stop_with_join(FdServer* server)
{
	if(!server)return;

	FdServer_stop(server);
	if(server->flag_pole & THREAD_CREATED)
	{
		pthread_join(server->thread, NULL);
		flag_lower(&server->flag_pole, THREAD_CREATED);
	}
}

/* Starts the server listening.  If the server was already running,
 * it will be closed before restarting.
 *
 * This function WILL block until the the server is closed.
 *
 * Parameters:
 * 		server: The server that will be listening for clients.
 *
 * Note:
 * 		Upon return, 'server's socket is guaranteed to be -1. */
alib_error FdServer_run(FdServer* server)
{
	alib_error err = ALIB_OK;

	if(server->ep.efd > -1 || server->sock > -1)
		FdServer_stop_with_join(server);

	err = run_init(server);
	if(err)
		return(err);

	flag_lower(&server->flag_pole, THREAD_STOP);
	err = run_loop(server);

	return(err);
}
/* Starts a thread.  If the server is already running, ALIB_OK will
 * be returned.
 *
 * Returns:
 * 		alib_error.
 * 		Anything else: Error code returned by pthread_create(). */
int FdServer_run_on_thread(FdServer* server)
{
	int err;

	if(!server)return(ALIB_BAD_ARG);

	if(FdServer_is_running(server))
		return(ALIB_OK);

	err = run_init(server);
	if(err)
		goto f_return;

	/* Set the thread flags and start the thread. */
	flag_lower(&server->flag_pole, THREAD_STOP);
	flag_raise(&server->flag_pole, THREAD_CREATED);
	err = pthread_create(&server->thread, NULL, start_thread, server);
	if(err)
		flag_lower(&server->flag_pole, THREAD_CREATED);

f_return:
	return(err);
}

		/* Getters */
/* Returns the socket for the server.
 *
 * Assumes server is not null. */
int FdServer_get_socket(FdServer* server)
{
	return(server->sock);
}
/* Returns the ArrayList of client sockets connected to the server.
 *
 * Assumes server is not null. */
const ArrayList* FdServer_get_client_sockets(FdServer* server)
{
	return(server->clients);
}
/* Returns 0 if the server is not running, !0 otherwise.
 *
 * Assumes server is not null. */
char FdServer_is_running(FdServer* server)
{
	return((server->sock > -1));
}
/* Returns the list of flags used by the object.
 *
 * Assumes server is not null. */
flag_pole FdServer_get_flag_pole(FdServer* server)
{
	return(server->flag_pole);
}

/* Returns the extended data of the server.
 *
 * Assumes 'server' is not null. */
void* FdServer_get_extended_data(FdServer* server)
{
	return(server->extended_data);
}

/* Returns the address struct for the FdServer.
 *
 * Assumes 'server' is not null. */
const struct sockaddr_un* FdServer_get_addr(FdServer* server)
{
	return(&server->addr);
}
		/***********/

		/* Setters */
/* Sets the on connect callback for the server.
 *
 * Assumes 'server' is not null. */
void FdServer_set_on_connect_cb(FdServer* server, fscb_on_connect on_connect)
{
	server->on_connect = on_connect;
}
/* Sets the on receive callback for the server.
 *
 * Assumes 'server' is not null. */
void FdServer_set_on_receive_cb(FdServer* server, fscb_on_receive on_receive)
{
	server->on_receive = on_receive;
}
/* Sets the on disconnect callback for the server.
 *
 * Assumes 'server' is not null. */
void FdServer_set_on_disconnect_cb(FdServer* server, fscb_on_disconnect on_disconnect)
{
	server->on_disconnect = on_disconnect;
}

/* Sets the extended data for the FdServer.
 *
 * Parameters:
 * 		server: The server who's extended data should be set.
 * 		extended_data: A pointer to the data that should be attached to
 * 			to the server.
 * 		free_extended_data_cb: The callback function used to free the
 * 			extended data.  This will be called whenever the server
 * 			object is destroyed or the extended data is is replaced.
 * 		free_old_data: If !0, the old extended_data will be freed before
 * 			moving on.  This only works if the the 'free_extended_data_cb'
 * 			had previously been set.
 *
 * Notes:
 * 		Assumes 'server' is not null.
 */
void FdServer_set_extended_data(FdServer* server, void* extended_data,
		alib_free_value free_extended_data_cb, char free_old_data)
{
	if(free_old_data && server->extended_data && server->free_extended_data)
		server->free_extended_data(server->extended_data);

	server->extended_data = extended_data;
	server->free_extended_data = free_extended_data_cb;
}
		/***********/
	/******************************/

	/*******Constructors*******/
/* Creates a new FdServer.
 *
 * Parameters:
 * 		fname: This is the temporary file name that will be used
 * 			by the UNIX file descriptor.  Cannot be larger than
 * 			sizeof(struct sockaddr_un.sun_path) (108 bytes as of 2015/9/28).
 * 			If the size does exeed this ammount, the data will be truncated.
 * 		name_len: The length of 'fname'.  If 0, the value will be calculated by
 * 			strlen(fname).
 * 		on_connect: (OPTIONAL) Called whenever a client connects to the server.
 * 		on_receive: (OPTIONAL) Called whenever data is received from the client.
 *
 * Returns:
 * 		NULL: Error.
 * 		FdServer*: Success.
 */
FdServer* newFdServer(const char* fname, size_t name_len,
		fscb_on_connect on_connect, fscb_on_receive on_receive)
{
    if(!fname)return(NULL);

	FdServer* server;

	/* Allocate object memory. */
	server = calloc(1, sizeof(FdServer));
	if(!server)return(NULL);

	/* Initialize threading members. */
	server->flag_pole = 0;

	/* Initialize callback members. */
	server->on_connect = on_connect;
	server->on_receive = on_receive;
	server->on_disconnect = NULL;

	/* Initialize extended data members. */
	server->extended_data = NULL;
	server->free_extended_data = NULL;

	/* Initialize address. */
	server->sock = -1;
	if(name_len == 0)
		name_len = strlen(fname);
	memset(&server->addr, 0, sizeof(server->addr));
	server->addr.sun_family = AF_UNIX;
	memcpy(server->addr.sun_path, fname, name_len);

	/* Initialize client socket list. */
	server->clients = newArrayList(free_client);

	/* Initialize non-dynamic members. */
	server->ep.efd = -1;

	/* Check for errors. */
	if(!server->clients)
		delFdServer(&server);

	return(server);
}

/* Deletes the FdServer and sets the pointer to NULL. */
void delFdServer(FdServer** server)
{
	if(!server || !*server)return;

	FdServer_stop_with_join(*server);
	delArrayList(&(*server)->clients);

	if((*server)->extended_data && (*server)->free_extended_data)
		(*server)->free_extended_data((*server)->extended_data);

	free(*server);
	*server = NULL;
}
	/**************************/
/**************************************/
