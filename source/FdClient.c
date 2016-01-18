#include "FdClient_private.h"

/*******Private Structs*******/
struct fdc_pack
{
	FdClient* client;
	fdc_fd_received_cb fd_received;
	fdc_fd_disconnect_cb fd_disconnected;
};
/*****************************/

/*******Private Functions*******/
void* threaded_listen(void* v_pack)
{
	struct fdc_pack* pack = (struct fdc_pack*)v_pack;

	flag_raise(&pack->client->flag_pole, THREAD_IS_RUNNING);
	FdClient_listen(pack->client, pack->fd_received);
	flag_lower(&pack->client->flag_pole, THREAD_IS_RUNNING);

	if(pack->client->sock < 0)
		pack->fd_disconnected(pack->client);

	free(pack);
	return(NULL);
}
/*******************************/

/*******Public Functions*******/
/* Closes an FdClient by disconnecting it from the host. */
void FdClient_close(FdClient* sender)
{
	if(!sender || sender->sock < 0)return;

	close(sender->sock);
	sender->sock = -1;
}

/* Connects the sender to the host. */
alib_error FdClient_connect(FdClient* sender)
{
	if(!sender)return(ALIB_BAD_ARG);

	/* Create the socket. */
	sender->sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if(sender->sock < 0)
		return(ALIB_FD_ERROR);

	/* Connect to the host. */
	if(connect(sender->sock, (struct sockaddr*)&sender->addr, sizeof(sender->addr)))
		goto f_error;

	return(ALIB_OK);
f_error:
	/* Error occurred, need to close the socket. */
	FdClient_close(sender);

	return(ALIB_FD_ERROR);
}

/* Sends a file descriptor to the host. */
alib_error FdClient_send(FdClient* sender, int fd)
{
	int err;

	if(!sender)return(ALIB_BAD_ARG);

	if(sender->sock < 0)
	{
		err = FdClient_connect(sender);
		if(err)return(err);
	}

	if(ancil_send_fd(sender->sock, fd))
		return(ALIB_CHECK_ERRNO);

	return(ALIB_OK);
}

/* Starts the FdClient listening for incoming data on the object's socket.
 *
 * Parameters:
 * 		client: (REQUIRED) The FdClient object to start listening.
 * 		fd_received: (REQUIRED) Called when data is received on the client's socket.
 * 		fd_disconnected: (OPTIONAL) Called whenever the client's socket is closed, either
 * 			on the host's side or the client's side.
 *
 * Returns:
 * 		ALIB_ERROR: Error that occurred during setup.
 * 		Anything else: Error code returned from pthread_create().
 * 		0: Success. */
alib_error FdClient_listen(FdClient* client, fdc_fd_received_cb fd_received)
{
	int rval;
	int new_fd;
	int efd;
	struct epoll_event event;

	/* Check passed arguments. */
	if(!client || !fd_received)return(ALIB_BAD_ARG);

	/* Ensure we are connected to the host. */
	if(client->sock < 0 && (rval = FdClient_connect(client)))
		return(rval);

	/* Setup epoll. */
	efd = epoll_create(1);
		if(efd < 0)return(ALIB_FD_ERROR);
	event.data.fd = client->sock;
	event.events = EPOLLIN;
	if(epoll_ctl(efd, EPOLL_CTL_ADD, client->sock, &event) < 0)
	{
		close(efd);
		return(ALIB_UNKNOWN_ERR);
	}

	/* Set the listening flag. */
	while(client->sock > -1)
	{
		/* Wait for an incoming message.  We put a timeout of 1 second
		 * as this will continue to block if, on the client side, the
		 * socket is closed. */
		rval = epoll_wait(efd, &event, 1, 1000);
		if(rval == 0)continue;

		/* Check for an epoll error on the socket. */
		if((event.events & (EPOLLERR | EPOLLHUP)) || !(event.events & EPOLLIN))
		{
			FdClient_close(client);
			break;
		}

		/* Receive the file descriptor. */
		if(ancil_recv_fd(client->sock, &new_fd) == 0 && new_fd != -1)
			fd_received(client, new_fd);
		else
			FdClient_close(client);
	}

	/* Cleanup. */
	close(efd);

	return(ALIB_OK);
}
/* Starts the FdClient listening for incoming data on the object's socket.
 * Listening will be run on a separate thread managed internally.
 *
 * Parameters:
 * 		client: (REQUIRED) The FdClient object to start listening.
 * 		fd_received: (REQUIRED) Called when data is received on the client's socket.
 * 		fd_disconnected: (OPTIONAL) Called whenever the client's socket is closed, either
 * 			on the host's side or the client's side.
 *
 * Returns:
 * 		ALIB_ERROR: Error that occurred during setup.
 * 		Anything else: Error code returned from pthread_create().
 * 		0: Success. */
int FdClient_listen_async(FdClient* client, fdc_fd_received_cb fd_received,
		fdc_fd_disconnect_cb fd_disconnected)
{
	struct fdc_pack* pack;

	/* Check for bad args. */
	if(!client || !fd_received)return(ALIB_BAD_ARG);

	/* If the thread is already running, there is nothing to do. */
	if(client->flag_pole & THREAD_IS_RUNNING)
		return(ALIB_OK);

	/* Ensure we are connected. */
	if(!FdClient_is_connected(client))
	{
		alib_error err = FdClient_connect(client);
		if(err)return(err);
	}

	/* Allocate memory for the arguments. */
	pack = malloc(sizeof(struct fdc_pack));
	if(!pack)return(ALIB_MEM_ERR);

	/* Set the arguments. */
	pack->client = client;
	pack->fd_received = fd_received;
	pack->fd_disconnected = fd_disconnected;

	/* Clean up the thread memory if it had previously been created. */
	if(client->flag_pole & THREAD_CREATED)
		pthread_join(client->thread, NULL);
	else
		flag_raise(&client->flag_pole, THREAD_CREATED);
	/* Run the thread. */
	return(pthread_create(&client->thread, NULL, threaded_listen, pack));
}

	/* Getters */
/* Returns the socket of the sender.
 *
 * Assumes 'sender' is not null. */
int FdClient_get_sock(FdClient* sender)
{
	return(sender->sock);
}
/* Returns the sockaddr struct for the FdClient.
 *
 * Assumes 'sender' is not null. */
const struct sockaddr_un* FdClient_get_addr(FdClient* sender)
{
	return(&sender->addr);
}

/* Returns the extended data member of the client.
 *
 * Assumes 'client' is not null. */
void* FdClient_get_extended_data(FdClient* client){return(client->extended_data);}
/* Returns 0 if the client is not connected, otherwise returned !0.
 *
 * Assumes 'client' is not null. */
char FdClient_is_connected(FdClient* client){return((client->sock < 0)?0:1);}
	/***********/

	/* Setters */
void FdClient_set_extended_data(FdClient* client, void* data, alib_free_value free_data,
		char free_old_data)
{
	if(!client)return;

	/* Free old data. */
	if(client->free_extended_data && client->extended_data && free_old_data)
		client->free_extended_data(client->extended_data);

	/* Set the members. */
	client->extended_data = data;
	client->free_extended_data = free_data;
}
	/***********/
/******************************/

/*******Constructors*******/
/* Allocates a new FdClient according to the given file name.
 *
 * Parameters:
 * 		fname: This is the unique file name that the receiver will
 * 			be reading from.  This is used instead of a port due to
 * 			the fact that we have to use AF_UNIX protocols.
 * 		name_len: This is the length of the given 'fname'.  If
 * 			the value is 0, length of 'fname' will be calculated
 * 			by calling 'strlen(fname)'.  If the length is larger than
 * 			the length of (struct sockaddr_un).sun_path, then the name
 * 			will be truncated to fit into the buffer.
 *
 * Returns:
 * 		NULL: Error.
 * 		FdClient*: Newly allocated FdClient object. */
FdClient* newFdClient(const char* fname, size_t name_len)
{
	FdClient* sender;

	/* Check for bad arguments. */
	if(!fname)return(NULL);

	/* Allocate object. */
	sender = malloc(sizeof(FdClient));
	if(!sender)return(NULL);

	/* Ensure name length is valid. */
	if(name_len == 0)
		name_len = strlen(fname);
	if(name_len > sizeof(sender->addr.sun_path))
		name_len = sizeof(sender->addr.sun_path);

	/* Initialize address. */
	memset(&sender->addr, 0, sizeof(sender->addr));
	sender->addr.sun_family = AF_UNIX;
	memcpy(sender->addr.sun_path, fname, name_len);

	/* Initialize other members. */
	sender->sock = -1;
	sender->extended_data = NULL;
	sender->free_extended_data = NULL;
	sender->flag_pole = 0;

	return(sender);
}

/* Frees an FdClient object and sets the pointer to null. */
void delFdClient(FdClient** sender)
{
	if(!sender || !*sender)return;

	FdClient_close(*sender);

	if((*sender)->flag_pole & THREAD_CREATED)
		pthread_join((*sender)->thread, NULL);
	if((*sender)->free_extended_data && (*sender)->extended_data)
		(*sender)->free_extended_data((*sender)->extended_data);

	free(*sender);
	*sender = NULL;
}
/**************************/
