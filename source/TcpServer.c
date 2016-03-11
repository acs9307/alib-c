#include "TcpServer_private.h"

/*******Private Functions*******/
	/* Callback Functions */
/* Called whenever a client is removed from the array list. */
static void remove_client_cb(void* v_sock_pack)
{
	socket_package* sp = (socket_package*)v_sock_pack;
	TcpServer* server = (TcpServer*)sp->parent;

	/* Call the server's client disconnected callback. */
	if(server->client_disconnected)
	{
		int rval = server->client_disconnected(server, sp);
		if(rval & SCB_RVAL_STOP_SERVER)
		{
			close(server->sock);
			server->sock = -1;
		}
	}

	/* Free the socket package. */
	close_and_free_socket_package(sp);
}
	/**********************/

/* Creates the server's socket and then calls bind() and listen()
 * on the socket. */
static alib_error bind_and_listen(TcpServer* server)
{
	int err;
	int optval = 1;

	if(!server)return(ALIB_BAD_ARG);

	if(server->sock > -1)
		return(ALIB_OK);

	/* Create the server's socket. */
	server->sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(server->sock < 0)
		return(ALIB_FD_ERR);
	setsockopt(server->sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

	/* Bind the server's socket. */
	err = bind(server->sock, (struct sockaddr*)&server->addr, sizeof(struct sockaddr_in));
	if(err)
	{
		err = ALIB_CHECK_ERRNO;
		goto f_error;
	}

	/* Start listening on the server's socket. */
	err = listen(server->sock, DEFAULT_BACKLOG_SIZE);
	if(err)
	{
		err = ALIB_CHECK_ERRNO;
		goto f_error;
	}

	return(ALIB_OK);
f_error:
	/* Error occurred, clean everything up. */
	if(server->sock > -1)
	{
		close(server->sock);
		server->sock = -1;
	}

	return(err);
}

/* Loop for listening for incoming events on a socket. */
static alib_error listen_loop(EpollPack* ep)
{
	if(!ep)return(ALIB_BAD_ARG);

	TcpServer* server = (TcpServer*)EpollPack_get_user_data(ep);

	if(!server || !ep)return(ALIB_BAD_ARG);

	int rval;
	int event_count;
	struct epoll_event* event_it;
	int data_in_count;
	void* data_in_buff = malloc(DEFAULT_INPUT_BUFF_SIZE);

	/* Ensure we were able to allocate the data in buffer. */
	if(!data_in_buff)return(ALIB_MEM_ERR);

	/* While our socket is open, then we will keep running. */
	while(!(server->flag_pole & THREAD_STOP) && server->sock > -1)
	{
		/* Wait for an event to come. */
		event_count = epoll_wait(EpollPack_get_efd(ep), EpollPack_get_triggered_events(ep), EpollPack_get_triggered_event_len(ep),
				server->epoll_wait_timeout);
		if(!event_count)continue;

		/* The the event_count is less than zero, then an error occurred. */
		if(event_count < 0)
		{
			if(server->flag_pole & THREAD_STOP)
				rval = ALIB_OK;
			else
			{
				if(server->sock > -1 && EpollPack_get_efd(ep) > -1)
					continue;
				rval = ALIB_CHECK_ERRNO;
			}

			goto f_return;
		}

		/* Iterate through the events. */
		for(event_it = EpollPack_get_triggered_events(ep); event_count > 0;
				++event_it, --event_count)
		{
			/* If the event is on the server's socket, that means we have an incoming client. */
			if(event_it->data.fd == server->sock)
			{
				int new_sock = accept(server->sock, NULL, NULL);
				socket_package* client_pack;

				/* If acceptance did not work, then we just move on to the next event. */
				if(new_sock < 0)continue;

				/* Create a new socket package for the client. */
				client_pack = new_socket_package(new_sock);
				if(!client_pack)
				{
					close(new_sock);
					rval = ALIB_MEM_ERR;
					goto f_return;
				}

				/* Call the client connected callback. */
				if(server->client_connected)
				{
					flag_raise(&server->flag_pole, OBJECT_CALLBACK_STATE);
					rval = server->client_connected(server, client_pack);
					flag_lower(&server->flag_pole, OBJECT_CALLBACK_STATE);

					if(server->flag_pole & OBJECT_DELETE_STATE)
					{
						rval = ALIB_OK;
						goto f_return;
					}

					if(rval & (SCB_RVAL_CLOSE_CLIENT | SCB_RVAL_STOP_SERVER))
						close_and_del_socket_package(&client_pack);
					if(rval & SCB_RVAL_STOP_SERVER)
					{
						rval = ALIB_OK;
						goto f_return;
					}
					if((rval & SCB_RVAL_HANDLED) || !client_pack)
						continue;
				}

				/* Add the client to the epoll list. */
				rval = EpollPack_add_sock(ep, EPOLLIN, client_pack->sock);
				if(rval < 0)
				{
					free_socket_package(client_pack);
					continue;
				}

				/* Add the client to the client list. */
				client_pack->parent = server;
				if(!ArrayList_add(server->client_list, client_pack))
				{
					free_socket_package(client_pack);
					rval = ALIB_MEM_ERR;
					goto f_return;
				}
			}
			/* Event occurred on a client socket. */
			else
			{
				/* Use compare_int_ptr as the first member in the socket package
				 * is an integer. */
				socket_package* client = (socket_package*)ArrayList_find_item_by_value_tsafe(
						server->client_list, &event_it->data.fd, compare_int_ptr);
				if(!client)
				{
					close(event_it->data.fd);
					continue;
				}

				/* Call the client_data_ready callback. */
				if(server->client_data_ready)
				{
					flag_raise(&server->flag_pole, OBJECT_CALLBACK_STATE);
					rval = server->client_data_ready(server, client, &data_in_buff,
							&data_in_count);
					flag_lower(&server->flag_pole, OBJECT_CALLBACK_STATE);

					/* Check delete state. */
					if(server->flag_pole & OBJECT_DELETE_STATE)
					{
						rval = ALIB_OK;
						goto f_return;
					}

					/* Check return value. */
					if(rval & SCB_RVAL_CLOSE_CLIENT)
						ArrayList_remove(server->client_list, client);
					if(rval & SCB_RVAL_STOP_SERVER)
					{
						rval = ALIB_OK;
						goto f_return;
					}
					if(rval & (SCB_RVAL_HANDLED | SCB_RVAL_CLOSE_CLIENT))
						continue;
				}
				else
					data_in_count = recv(client->sock, data_in_buff,
							DEFAULT_INPUT_BUFF_SIZE, 0);

				/* If the client's socket was closed, then we just remove it
				 * from the list. */
				if(data_in_count < 1)
					ArrayList_remove(server->client_list, client);
				/* Call the client data in callback. */
				else if(server->client_data_in)
				{
					flag_raise(&server->flag_pole, OBJECT_CALLBACK_STATE);
					rval = server->client_data_in(server, client, data_in_buff,
							data_in_count);
					flag_lower(&server->flag_pole, OBJECT_CALLBACK_STATE);

					/* Check object delete state. */
					if(server->flag_pole & OBJECT_DELETE_STATE)
					{
						rval = ALIB_OK;
						goto f_return;
					}

					/* Check callback return value. */
					if(rval & SCB_RVAL_CLOSE_CLIENT)
						ArrayList_remove(server->client_list, client);
					if(rval & SCB_RVAL_STOP_SERVER)
					{
						rval = ALIB_OK;
						goto f_return;
					}
				}
			}
		}
	}
	rval = ALIB_OK;

f_return:
	if(data_in_buff)
		free(data_in_buff);
	ArrayList_clear_tsafe(server->client_list);

	return(rval);
}

/* Starts the listening thread for the server. */
static void start_thread(EpollPack* ep)
{
	if(!ep)return;

	TcpServer* server = (TcpServer*)EpollPack_get_user_data(ep);
	if(!server)return;

	flag_raise(&server->flag_pole, THREAD_IS_RUNNING);
	pthread_cond_broadcast(&server->event_cond);

	listen_loop(ep);
	if(server->flag_pole & OBJECT_DELETE_STATE)
	{
		flag_lower(&server->flag_pole, THREAD_IS_RUNNING);
		freeTcpServer(server);
		goto f_cleanup;
	}

	/* Call the thread returning event. */
	if(server->thread_returning)
	{
		flag_raise(&server->flag_pole, OBJECT_CALLBACK_STATE);
		server->thread_returning(server);
		flag_lower(&server->flag_pole, OBJECT_CALLBACK_STATE);

		if(server->flag_pole & OBJECT_DELETE_STATE)
		{
			flag_lower(&server->flag_pole, THREAD_IS_RUNNING);
			freeTcpServer(server);
			goto f_cleanup;
		}
	}

	TcpServer_stop_async(server);
	flag_lower(&server->flag_pole, THREAD_IS_RUNNING);
	pthread_cond_broadcast(&server->event_cond);

f_cleanup:
	/* Cleanup. */
	if(ep)
		delEpollPack(&ep);
}
/*******************************/

/*******Public Functions*******/
/* Starts the TcpServer on the current thread.  If the server is already
 * running, it will first be stopped then restarted. To prevent this behavior, first
 * check 'TcpServer_is_running()'.
 *
 * WILL BLOCK until previous instance of 'server' has finished running. */
alib_error TcpServer_start(TcpServer* server)
{
	if(!server)return(ALIB_BAD_ARG);
		else if((server->flag_pole & OBJECT_DELETE_STATE))
			return(ALIB_STATE_ERR);

	int err;
	struct EpollPack* ep = newEpollPack(0, server, NULL);
	if(!ep)
	{
		err = ALIB_FD_ERR;
		goto f_return;
	}

	/* Here we stop the server as this is a blocking function.
	 * Instead of returning immediately with ALIB_OK if the server is
	 * already running, we simply shut it down then restart it.
	 * If this behavior is not desired, the user should first call
	 * TcpServer_is_running() before deciding whether or not to call this function. */
	TcpServer_stop(server);
	flag_lower(&server->flag_pole, THREAD_STOP);

	/* Create the socket. */
	err = bind_and_listen(server);
	if(err)return(err);

	/* Initialize epoll. */
	err = EpollPack_add_sock(ep, EPOLLIN, server->sock);
	if(err)goto f_return;

	/* Start listening. */
	err = listen_loop(ep);
	if(server->flag_pole & OBJECT_DELETE_STATE)
		return(err);

f_return:
	TcpServer_stop(server);
	delEpollPack(&ep);

	return(err);
}
/* Starts the TcpServer on a separate thread.
 * If the server is already running, ALIB_OK is returned. */
alib_error TcpServer_start_async(TcpServer* server)
{
	if(!server)return(ALIB_BAD_ARG);
	else if((server->flag_pole & OBJECT_DELETE_STATE))
		return(ALIB_STATE_ERR);

	/* If the thread is already running, then we don't need to do anything. */
	if((server->flag_pole & THREAD_IS_RUNNING) &&
			!(server->flag_pole & THREAD_IS_RUNNING))
		return(ALIB_OK);
	else if((server->flag_pole & THREAD_STOP) &&
			(server->flag_pole & THREAD_IS_RUNNING))
		TcpServer_wait_for_thread_return(server);

	/* Allocate memory for the epoll_pack. */
	EpollPack* ep = NULL;

	/* Bind the socket. */
	int err = bind_and_listen(server);
	if(err)return(err);

	/* Initialize the epoll package. */
	ep = newEpollPack(0, server, NULL);
	if(!ep)goto f_error;

	err = EpollPack_add_sock(ep, EPOLLIN, server->sock);
	if(err)goto f_error;

	/* Start the thread. */
	flag_lower(&server->flag_pole, THREAD_STOP);
	flag_raise(&server->flag_pole, THREAD_CREATED | THREAD_IS_RUNNING);
	if(pthread_create(&server->event_thread, NULL, (pthread_proc)start_thread, ep))
	{
		flag_lower(&server->flag_pole, THREAD_CREATED | THREAD_IS_RUNNING);
		err = ALIB_THREAD_ERR;
		goto f_error;
	}

	return(ALIB_OK);

f_error:
	TcpServer_stop_async(server);
	delEpollPack(&ep);

	return(err);
}

/* Stops the TcpServer.
 *
 * WILL BLOCK until the listener thread has been stopped.
 * Safe to call in callbacks. */
void TcpServer_stop(TcpServer* server)
{
	TcpServer_stop_async(server);

	/* Wait for the thread to stop if we are not in a callback state. */
	if(!(server->flag_pole & OBJECT_CALLBACK_STATE))
		TcpServer_wait_for_thread_return(server);
}
/* Requests that the server loop be stopped and returns immediately.
 *
 * Safe to call in callbacks. */
void TcpServer_stop_async(TcpServer* server)
{
	/* We need to close the socket to signify that the server is shutting down. */
	if(server->sock > -1)
	{
		close(server->sock);
		server->sock = -1;
	}

	/* If a thread has been created, then we need to join it. */
	flag_raise(&server->flag_pole, THREAD_STOP);
	pthread_cond_broadcast(&server->event_cond);
	if(server->flag_pole & THREAD_CREATED)
	{
		pthread_detach(server->event_thread);
		flag_lower(&server->flag_pole, THREAD_CREATED);
	}
}

/* Waits for the server thread to return before returning.
 * If the server was not run using 'TcpServer_start_async()', then the
 * function returns immediately.
 *
 * If called from callback, function returns immediately. */
void TcpServer_wait_for_thread_return(TcpServer* server)
{
	if(!server || (server->flag_pole & OBJECT_CALLBACK_STATE))return;

	if(pthread_mutex_lock(&server->event_mutex))
		return;
	while(server->flag_pole & THREAD_IS_RUNNING)
		pthread_cond_wait(&server->event_cond, &server->event_mutex);
	pthread_mutex_unlock(&server->event_mutex);
}

	/* Getters */
/* Returns the socket of the server.
 *
 * Assumes 'server' is not null. */
int TcpServer_get_sock(const TcpServer* server){return(server->sock);}
/* Returns the sockaddr_in struct of the server.
 *
 * Though it is suggested not to modify this struct, sometimes the need may arise.
 * For modifications to be effective, the caller must make changes before starting
 * the server, otherwise behavior is undefined.
 *
 * Assumes 'server' is not null. */
const struct sockaddr_in* TcpServer_get_addr(const TcpServer* server){return(&server->addr);}
/* Returns 0 if the server is not running, otherwise !0.
 *
 * Assumes 'server' is not null. */
char TcpServer_is_running(const TcpServer* server){return((server->sock > -1));}
/* Returns the flag pole of the server.
 *
 * Assumes 'server' is not null. */
flag_pole TcpServer_get_flag_pole(const TcpServer* server){return(server->flag_pole);}
/* Returns the number of milliseconds 'epoll_wait()' will wait before checking
 * the status of the server.
 *
 * Assumes 'server' is not null. */
int TcpServer_get_epoll_wait_timeout(const TcpServer* server){return(server->epoll_wait_timeout);}
/* Returns a constant list of clients that are currently connected to the server.
 *
 * Assumes 'server' is not null. */
const ArrayList* TcpServer_get_client_list(const TcpServer* server){return(server->client_list);}
/* Returns the extended data of the server.
 *
 * Assumes 'server' is not null. */
void* TcpServer_get_extended_data(const TcpServer* server){return(server->ex_data);}
	/***********/

	/* Setters */
/* Sets the timeout for 'epoll_wait()'.  This is the number of milliseconds
 * the server will wait before checking the server state in the listen thread.
 *
 * If started using 'TcpServer_start()' and the server is not stopped except in
 * the callback functions, then it is safe to set the timeout to '-1'.
 *
 * Default value is 1 second.
 *
 * Parameters:
 * 		server: The object to modify.
 * 		timeout_millis: The number of milliseconds to wait before returning
 * 			from 'epoll_wait()'.  If -1, the timeout is equal to infinity. */
void TcpServer_set_epoll_wait_timeout(TcpServer* server, int timeout_millis)
{
	if(timeout_millis < 0)
		timeout_millis = -1;
	server->epoll_wait_timeout = timeout_millis;
}

/* Sets the callback for when a client connects to the server.
 *
 * Assumes 'server' is not null. */
void TcpServer_set_client_connected_cb(TcpServer* server,
		ts_client_connected_cb client_connected)
{
	server->client_connected = client_connected;
}
/* Sets the callback for when data is ready on a client socket.
 *
 * Assumes 'server' is not null. */
void TcpServer_set_client_data_ready_cb(TcpServer* server,
		ts_client_data_ready_cb client_data_ready)
{
	server->client_data_ready = client_data_ready;
}
/* Sets the callback for when data is received from a client.
 *
 * Assumes 'server' is not null. */
void TcpServer_set_client_data_in_cb(TcpServer* server,
		ts_client_data_in_cb client_data_in)
{
	server->client_data_in = client_data_in;
}
/* Sets the callback for when a client disconnects from the server.
 *
 * Assumes 'server' is not null. */
void TcpServer_set_client_disconnected_cb(TcpServer* server,
		ts_client_disconnected_cb client_disconnected)
{
	server->client_disconnected = client_disconnected;
}
/* Sets the callback for when the listening thread is about to return.
 *
 * Assumes 'server' is not null. */
void TcpServer_set_thread_returning_cb(TcpServer* server,
        ts_thread_returning_cb cb)
{
    server->thread_returning = cb;
}

/* Sets the the extended data for the server.
 *
 * Assumes 'server' is not null.
 *
 * Parameters:
 * 		server: The server to modify.
 * 		ex_data: The data to set for the server's extended data.
 * 		free_data: The callback function used to free the extended data when
 * 			it is no longer needed.
 * 		free_old_data: If !0, the server will automatically call the free_data_cb
 * 			on the old extended data if possible. */
void TcpServer_set_extended_data(TcpServer* server, void* ex_data,
		alib_free_value free_data, char free_old_data)
{
	if(free_old_data && server->free_data_cb && server->ex_data)
		server->free_data_cb(server->ex_data);

	server->ex_data = ex_data;
	server->free_data_cb = free_data;
}
	/***********/
/******************************/

/*******Constructors*******/
/* Creates a new TcpServer but does not start the server nor bind the server's socket.
 * To start the server, call TcpServer_start() or TcpServer_start_async(). */
TcpServer* newTcpServer(uint16_t port, void* ex_data,
		alib_free_value free_data_cb)
{
	TcpServer* server = malloc(sizeof(TcpServer));
	if(!server)return(NULL);

	/* Setup the address struct. */
	memset(&server->addr, 0, sizeof(struct sockaddr));
	server->addr.sin_addr.s_addr = INADDR_ANY;
	server->addr.sin_family = AF_INET;
	server->addr.sin_port = htons(port);

	/* Initialize other members. */
	server->sock = -1;
	server->flag_pole = FLAG_INIT;
	server->ex_data = ex_data;
	server->epoll_wait_timeout = 1000;
	server->free_data_cb = free_data_cb;
	server->event_mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
	server->event_cond = (pthread_cond_t)PTHREAD_COND_INITIALIZER;

	/* Initialize callback pointers. */
	server->client_connected = NULL;
	server->client_data_in = NULL;
	server->client_disconnected = NULL;
	server->client_data_ready = NULL;
	server->thread_returning = NULL;

	/* Initialize dynamic members. */
	server->client_list = newArrayList(remove_client_cb);

	/* Check dynamic members. */
	if(!server->client_list)
		delTcpServer(&server);

	return(server);
}
void freeTcpServer(TcpServer* server)
{
	if(!server)return;

	TcpServer_stop(server);
	flag_raise(&server->flag_pole, OBJECT_DELETE_STATE);

	/* Ensure the thread is not running and that we are
	 * no longer in a callback state before freeing members. */
	if(!(server->flag_pole & THREAD_IS_RUNNING) &&
			!(server->flag_pole & OBJECT_CALLBACK_STATE))
	{
		/* Ensure the user didn't restart the server after being in a delete state. */
		TcpServer_stop(server);

		flag_raise(&server->flag_pole, OBJECT_CALLBACK_STATE);
		if(server->free_data_cb)
			server->free_data_cb(server->ex_data);
		flag_lower(&server->flag_pole, OBJECT_CALLBACK_STATE);

		delArrayList(&server->client_list);
		free(server);
	}
}
void delTcpServer(TcpServer** server)
{
	if(!server)return;

	freeTcpServer(*server);
	*server = NULL;
}
/**************************/
