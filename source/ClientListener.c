#include "includes/ClientListener_private.h"

/*******Private Functions*******/
	/* T-safe functions */
/* Closes the epoll by first locking the mutex. */
static alib_error close_epoll_tsafe(ClientListener* listener)
{
	if(pthread_mutex_lock(&listener->mutex))
		return(ALIB_MUTEX_ERR);

	close(listener->ep.efd);
	listener->ep.efd = -1;
	pthread_cond_broadcast(&listener->t_cond);

	if(pthread_mutex_unlock(&listener->mutex))
		return(ALIB_MUTEX_ERR);

	return(ALIB_OK);
}
	/********************/

	/* Callback Functions */
/* Called whenever a client is removed from the array list. */
static void remove_client_cb(void* v_sock_pack)
{
	socket_package* sp = (socket_package*)v_sock_pack;
	ClientListener* listener = (ClientListener*)sp->parent;

	/* Call the server's client disconnected callback. */
	if(listener->disconnected)
	{
		int rval = listener->disconnected(listener, sp);
		if(rval & SCB_RVAL_STOP_SERVER)
		{
			flag_raise(&listener->flag_pole, THREAD_STOP);
			close(listener->ep.efd);
			listener->ep.efd = -1;
		}
	}

	/* Free the socket package. */
	close_and_free_socket_package(sp);
	pthread_cond_broadcast(&listener->t_cond);
}
	/**********************/

/* Initializes an an epoll_pack struct. */
static alib_error init_epoll(struct epoll_pack* pack)
{
	/* Make the epoll fd. */
	pack->efd = epoll_create(DEFAULT_BACKLOG_SIZE);
	if(pack->efd < 0)
		return(ALIB_FD_ERR);

	/* Setup triggered event list. */
	memset(pack->triggered_events, 0, sizeof(pack->triggered_events));

	return(ALIB_OK);
}
/* Thread safe version of 'init_epoll()'. */
static alib_error init_epoll_tsafe(ClientListener* listener)
{
	if(!listener)return(ALIB_BAD_ARG);

	alib_error err;

	if(pthread_mutex_lock(&listener->mutex))
		return(ALIB_MUTEX_ERR);

	err = init_epoll(&listener->ep);

	if(pthread_mutex_unlock(&listener->mutex))
		return(ALIB_MUTEX_ERR);
	return(err);
}

/* Adds a single socket to the epoll list. */
static alib_error add_sock_to_epoll(struct epoll_pack* pack, int sock)
{
	if(!pack || sock < 0 || pack->efd < 0)return(ALIB_BAD_ARG);

	pack->event.data.fd = sock;
	pack->event.events = EPOLLIN;
	if(epoll_ctl(pack->efd, EPOLL_CTL_ADD, sock,
			&pack->event) < 0)
		return(ALIB_UNKNOWN_ERR);
	else
		return(ALIB_OK);
}
/* Thread safe version of 'add_sock_to_epoll()'. */
static alib_error add_sock_to_epoll_tsafe(ClientListener* listener, int sock)
{
	int err;

	if(!listener)return(ALIB_BAD_ARG);

	if(pthread_mutex_lock(&listener->mutex))
		return(ALIB_MUTEX_ERR);

	err = add_sock_to_epoll(&listener->ep, sock);

	if(pthread_mutex_unlock(&listener->mutex))
		return(ALIB_MUTEX_ERR);

	return(err);
}
/* Adds all the clients from the listener to the epoll list. */
static alib_error add_clients_to_epoll(ClientListener* listener)
{
	if(!listener || listener->ep.efd < 0)return(ALIB_BAD_ARG);

	int rval;
	socket_package** array_ptr;
	size_t array_count;

	/* Setup epoll to listen to all clients. */
	array_ptr = (socket_package**)ArrayList_get_array_ptr(listener->client_list);
	for(array_count = ArrayList_get_count(listener->client_list);
			array_count > 0; ++array_ptr)
	{
		if(!*array_ptr)
			continue;
		else
			--array_count;

		if((rval = add_sock_to_epoll(&listener->ep, (*array_ptr)->sock)))
			return(rval);
	}
	pthread_cond_broadcast(&listener->t_cond);

	return(ALIB_OK);
}
/* Thread safe version of 'add_clients_to_epoll()'. */
static alib_error add_clients_to_epoll_tsafe(ClientListener* listener)
{
	int err;

	if(!listener)return(ALIB_BAD_ARG);

	if(pthread_mutex_lock(&listener->mutex))
		return(ALIB_MUTEX_ERR);

	err = add_clients_to_epoll(listener);

	if(pthread_mutex_unlock(&listener->mutex))
		return(ALIB_MUTEX_ERR);

	return(err);
}

/* The main listening loop for the object. */
static alib_error listen_loop(ClientListener* listener)
{
	if(!listener)return(ALIB_BAD_ARG);

	int rval;
	int event_count;
	struct epoll_event* event_it;
	long data_in_count;
	void* data_in_buff = malloc(DEFAULT_INPUT_BUFF_SIZE);

	/* Ensure we were able to allocate the data in buffer. */
	if(!data_in_buff)
	{
		rval = ALIB_MEM_ERR;
		goto f_return;
	}

	/* While our socket is open, then we will keep running. */
	while(!(listener->flag_pole & THREAD_STOP))
	{
		/* If the array list is empty, then we simply wait until something is added
		 * to it or our thread is called to stop. */
		if(!ArrayList_get_count(listener->client_list))
		{
			/* Call the empty list callback. */
			if(listener->client_list_empty)
			{
				int rval = listener->client_list_empty(listener);
				if(rval & SCB_RVAL_STOP_SERVER)
					break;
			}

			pthread_mutex_lock(&listener->mutex);
			while(!ArrayList_get_count(listener->client_list) && !(listener->flag_pole & THREAD_STOP))
				pthread_cond_wait(&listener->t_cond, &listener->mutex);
			pthread_mutex_unlock(&listener->mutex);
			continue;
		}

		/* Wait for an event to come. */
		event_count = epoll_wait(listener->ep.efd, listener->ep.triggered_events, DEFAULT_BACKLOG_SIZE,
				1000);
		if(!event_count)continue;

		/* The the event_count is less than zero, then an error occurred. */
		if(event_count < 0)
		{
			if(listener->flag_pole & THREAD_STOP)
				rval = ALIB_OK;
			else
			{
				if(listener->ep.efd > -1)
					continue;
				rval = ALIB_CHECK_ERRNO;
			}

			goto f_return;
		}

		/* Iterate through the events. */
		if(pthread_mutex_lock(&listener->mutex))
		{
			rval = ALIB_MUTEX_ERR;
			goto f_return;
		}
		for(event_it = listener->ep.triggered_events; event_count > 0; ++event_it, --event_count)
		{
			/* Use compare_int_ptr as the first member in the socket package
			 * is an integer. */
			socket_package* client = (socket_package*)ArrayList_find_item_by_value_tsafe(
					listener->client_list, &event_it->data.fd, compare_int_ptr);
			if(!client)
			{
				close(listener->ep.triggered_events->data.fd);
				continue;
			}

			/* Error occurred on the socket. */
			if((event_it->events & (EPOLLERR | EPOLLHUP)) ||
					!(event_it->events & EPOLLIN))
			{
				ArrayList_remove_tsafe(listener->client_list, client);
				continue;
			}

			/* Call the client_data_ready callback. */
			if(listener->data_ready)
			{
				rval = listener->data_ready(listener, client, &data_in_buff,
						&data_in_count);
				if(rval & SCB_RVAL_CLOSE_CLIENT)
					ArrayList_remove_tsafe(listener->client_list, client);
				if(rval & SCB_RVAL_STOP_SERVER)
				{
					rval = ALIB_OK;
					flag_raise(&listener->flag_pole, THREAD_STOP);
					if(pthread_mutex_unlock(&listener->mutex))
						rval = ALIB_MUTEX_ERR;
					goto f_return;
				}

				if(rval & (SCB_RVAL_HANDLED | SCB_RVAL_CLOSE_CLIENT))
					continue;

			}
			else
			{
				data_in_count = recv(client->sock, data_in_buff,
						DEFAULT_INPUT_BUFF_SIZE, 0);
			}

			/* If the client's socket was closed, then we just remove it
			 * from the list. */
			if(data_in_count < 1)
				ArrayList_remove_tsafe(listener->client_list, client);
			/* Call the client data in callback. */
			else if(listener->data_in)
			{
				rval = listener->data_in(listener, client, data_in_buff,
						data_in_count);
				if(rval & SCB_RVAL_CLOSE_CLIENT)
					ArrayList_remove_tsafe(listener->client_list, client);
				if(rval & SCB_RVAL_STOP_SERVER)
				{
					rval = ALIB_OK;
					if(pthread_mutex_unlock(&listener->mutex))
						rval = ALIB_MUTEX_ERR;
					goto f_return;
				}
			}
		}
		if(pthread_mutex_unlock(&listener->mutex))
		{
			rval = ALIB_MUTEX_ERR;
			goto f_return;
		}
	}

	rval = ALIB_OK;

f_return:
	if(data_in_buff)
		free(data_in_buff);

	return(rval);
}

/* Function used to start listening to client events on a separate
 * thread. */
static void* threaded_run(void* v_listener)
{
	ClientListener* listener = (ClientListener*)v_listener;

	flag_raise(&listener->flag_pole, THREAD_IS_RUNNING);
	flag_lower(&listener->flag_pole, THREAD_STOP);
	listen_loop(listener);
	flag_lower(&listener->flag_pole, THREAD_IS_RUNNING);

	/* Epoll is no longer in use, we should close
	 * the file descriptor. */
	close_epoll_tsafe(listener);
	return(NULL);
}
/*******************************/

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
		socket_package* client_pack, char run_async)
{
	/* Check for bad arguments. */
	if(!listener || !client_pack)return(ALIB_BAD_ARG);

	int err;

	/* Add the client to the list. */
	if(!ArrayList_add_tsafe(listener->client_list, client_pack))
		return(ALIB_MEM_ERR);

	/* If the listener is already running, then we add it to the epoll list. */
	if((listener->flag_pole & THREAD_IS_RUNNING) && listener->ep.efd > -1)
	{
		err = add_sock_to_epoll_tsafe(listener, client_pack->sock);
		if(err)return(err);
	}

	/* Let our thread/s know to wake up. */
	pthread_cond_broadcast(&listener->t_cond);

	/* See if we want to ensure the listener is running. */
	if(run_async)
		return(ClientListener_start_async(listener));
	else
		return(ALIB_OK);
}
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
		char run_async, void* user_data, alib_free_value free_user_data)
{
	/* Check for bad arguments. */
	if(!listener || sock < 0)return(ALIB_BAD_ARG);

	int err;

	/* Allocate a new socket package. */
	socket_package* client_pack = new_socket_package(sock);
	if(!client_pack)return(ALIB_MEM_ERR);

	/* Set the package members to given values. */
	client_pack->parent = listener;
	client_pack->user_data = user_data;
	client_pack->free_user_data = free_user_data;

	/* Add the socket package to the list. */
	err = ClientListener_add_socket_package(listener, client_pack, run_async);
	if(err)
		free_socket_package(client_pack);
	return(err);
}

/* Removes the socket package from the listener and closes the socket.
 *
 * Assumes 'listener' and 'sp' is not null. */
void ClientListener_remove_socket_package(ClientListener* listener, socket_package* sp)
{
	ArrayList_remove(listener->client_list, sp);
}
/* Removes a socket from the listener list, it is suggested to use
 * ClientListener_remove_socket_package() whenever possible as it
 * is much more efficient.
 *
 * Assumes 'listener' is not null. */
void ClientListener_remove(ClientListener* listener, int sock)
{
	ClientListener_remove_socket_package(listener, (socket_package*)
			ArrayList_find_item_by_value(listener->client_list, &sock, compare_int_ptr));
}

/* Starts listening for incoming events from the the object's clients.
 * This function BLOCKS until either 'ClientListener_stop()' is called
 * or all the clients have disconnected and there are no more clients in
 * the listener's client list.
 */
alib_error ClientListener_start(ClientListener* listener)
{
	if(!listener)return(ALIB_BAD_ARG);

	int err;

	/* Stop the listener if it is running. */
	if((listener->flag_pole & THREAD_IS_RUNNING) || listener->ep.efd > -1)
		ClientListener_stop(listener);

	/* Initialize the epoll. */
	err = init_epoll_tsafe(listener);
	if(err)	return(err);
	/* Add the clients to the epoll. */
	err = add_clients_to_epoll_tsafe(listener);
	if(err)goto f_error;

	/* Start the listener. */
	err = listen_loop(listener);

	/* Must close the epoll. */
	close_epoll_tsafe(listener);

	return(err);
f_error:
	ClientListener_stop(listener);

	return(err);
}
/* Starts clients in the ClientListener on a separate thread.  The
 * thread is managed internally and can be stopped by calling
 * 'ClientListener_stop()' or 'delClientListener()'.
 *
 * Returns:
 * 		ALIB_OK: Thread is already running, or thread was started without error.
 * 			Any error that occurs after this was raised in the listen loop.
 * 		Anything else: Error code related to the error.
 */
alib_error ClientListener_start_async(ClientListener* listener)
{
	if(!listener)return(ALIB_BAD_ARG);

	int err = ALIB_OK;
	/* Check to see if the thread is running. */
	if(listener->flag_pole & THREAD_IS_RUNNING)
	{
		/* If we are supposed to stop the thread, then stop the
		 * ClientListener. */
		if(listener->flag_pole & THREAD_STOP)
			ClientListener_stop(listener);
		/* Otherwise, the thread is already running, there is nothing to do. */
		else
			return(ALIB_OK);
	}

	flag_lower(&listener->flag_pole, THREAD_STOP);

	/* Initialize the epoll. */
	err = init_epoll_tsafe(listener);
	if(err)	return(err);
	/* Add the clients to the epoll. */
	err = add_clients_to_epoll_tsafe(listener);
	if(err)goto f_error;


	/* Create the thread. */
	if(listener->flag_pole & THREAD_CREATED)
		pthread_join(listener->thread, NULL);
	else
		flag_raise(&listener->flag_pole, THREAD_CREATED);

	if(pthread_create(&listener->thread, NULL, threaded_run, listener))
	{
		/* If creation failed, then we need to return an error. */
		flag_lower(&listener->flag_pole, THREAD_CREATED);
		err = ALIB_THREAD_ERR;
		goto f_error;
	}

	return(ALIB_OK);
f_error:
	ClientListener_stop(listener);

	return(err);
}

/* Stops a currently running ClientListener object.  If the object
 * is already stopped, nothing will happen.
 *
 * This should be called whenever the object is no longer running. */
alib_error ClientListener_stop(ClientListener* listener)
{
	if(!listener)return(ALIB_BAD_ARG);

	/* Set the stop flag for the thread. */
	if(listener->flag_pole & THREAD_IS_RUNNING)
		flag_raise(&listener->flag_pole, THREAD_STOP);

	/* Close the epoll socket to break epoll_wait(). */
	close_epoll_tsafe(listener);

	/* If a thread has been created, join it. */
	if(listener->flag_pole & THREAD_CREATED)
	{
		pthread_cond_broadcast(&listener->t_cond);
		pthread_join(listener->thread, NULL);
		flag_lower(&listener->flag_pole, THREAD_CREATED);
	}

	return(ALIB_OK);
}
/* Waits until there are no more clients before calling ClientListener_stop()
 * on the object.  This will block until there are no more clients in the
 * list. */
alib_error ClientListener_stop_when_empty(ClientListener* listener)
{
	/* Check for bad arguments. */
	if(!listener)return(ALIB_BAD_ARG);

	/* If the thread is running, then */
	if(ArrayList_get_count(listener->client_list))
	{
		pthread_mutex_lock(&listener->mutex);
		while(ArrayList_get_count(listener->client_list))
			pthread_cond_wait(&listener->t_cond, &listener->mutex);
		pthread_mutex_unlock(&listener->mutex);
	}
	return(ClientListener_stop(listener));
}

	/* Getters */
/* Returns a constant pointer to the ClientListener's client list.
 * This should not be modified unless you know what you are doing as
 * it may corrupt the object.
 *
 * Clients are stored as 'socket_package' structs.
 *
 * Assumes 'listener' is not null. */
const ArrayList* ClientListener_get_client_list(ClientListener* listener)
{
	return(listener->client_list);
}
/* Returns the extended data of the ClientListener object.
 *
 * Assumes 'listener' is not null. */
void* ClientListener_get_extended_data(ClientListener* listener)
{
	return(listener->ex_data);
}
/* Returns the flag pole of the ClientListener object.
 *
 * Assumes 'listener' is not null. */
object_flag_pole ClientListener_get_flag_pole(ClientListener* listener)
{
	return(listener->flag_pole);
}
/* Returns the !0 if the object is listening for clients, 0 otherwise.
 *
 * Assumes 'listener' is not null. */
char ClientListener_is_listening(ClientListener* listener)
{
	return((listener->ep.efd > -1)?1:0);
}
	/***********/

	/* Setters */
/* Sets the 'client_data_ready' callback for the ClientListener.
 *
 * Assumes 'listener' is not null. */
void ClientListener_set_client_data_ready_cb(ClientListener* listener,
		cl_client_data_ready_cb data_ready)
{
	listener->data_ready = data_ready;
}
/* Sets the 'client_data_in' callback for the ClientListener.
 *
 * Assumes 'listener' is not null. */
void ClientListener_set_client_data_in_cb(ClientListener* listener,
		cl_client_data_in_cb data_in)
{
	listener->data_in = data_in;
}
/* Sets the 'client_disconnected' callback for the ClientListener.
 *
 * Assumes 'listener' is not null. */
void ClientListener_set_client_disconnected_cb(ClientListener* listener,
		cl_client_disconnected_cb disconnected)
{
	listener->disconnected = disconnected;
}
/* Sets the 'client_list_empty' callback for the ClientListener.
 *
 * Assumes 'listener' is not null. */
void ClientListener_set_client_list_empty_cb(ClientListener* listener,
		cl_client_list_empty_cb list_empty)
{
	listener->client_list_empty = list_empty;
}

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
		void* ex_data, alib_free_value free_ex_data, char free_old_data)
{
	/* Free the old extended data if we need to. */
	if(free_old_data && listener->free_extended_data && listener->ex_data)
		listener->free_extended_data(listener->ex_data);

	listener->ex_data = ex_data;
	listener->free_extended_data = free_ex_data;
}
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
ClientListener* newClientListener(void* ex_data, alib_free_value free_extended_data)
{
	ClientListener* listener = malloc(sizeof(ClientListener));
	if(!listener)return(NULL);

	/* Initialize callbacks. */
	listener->data_ready = NULL;
	listener->data_in = NULL;
	listener->disconnected = NULL;
	listener->client_list_empty = NULL;

	/* Initialize non-dynamic members. */
	listener->ex_data = ex_data;
	listener->free_extended_data = free_extended_data;
	listener->flag_pole = 0;

	memset(&listener->ep, 0, sizeof(listener->ep));
	listener->ep.efd = -1;

	/* We don't check for errors with pthread initialization as
	 * they should simply be setting the memory to zero.
	 * We use the initialization functions to follow convention.
	 * If we were to add attributes, then we would need to check for error. */
	pthread_mutex_init(&listener->mutex, NULL);
	pthread_cond_init(&listener->t_cond, NULL);

	/* Initialize dynamic members. */
	listener->client_list = newArrayList(remove_client_cb);

	/* Check for errors. */
	if(!listener->client_list)
		delClientListener(&listener);

	return(listener);
}

/* Destroys a ClientListener object and sets the pointer to NULL. */
void delClientListener(ClientListener** listener)
{
	if(!listener || !*listener)
		return;

	/* Notify our threads that we are deleting the object. */
	flag_raise(&(*listener)->flag_pole, OBJECT_DELETE_STATE);
	pthread_cond_broadcast(&(*listener)->t_cond);

	/* Delete dynamic members. */
	delArrayList(&(*listener)->client_list);

	/* Close open file descriptors. */
	if((*listener)->ep.efd)
		close((*listener)->ep.efd);

	/* Call destruction callbacks. */
	if((*listener)->free_extended_data && (*listener)->ex_data)
		(*listener)->free_extended_data((*listener)->ex_data);

	pthread_mutex_destroy(&(*listener)->mutex);
	pthread_cond_destroy(&(*listener)->t_cond);

	free(*listener);
	*listener = NULL;
}
/**************************/
