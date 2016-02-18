#include "TcpClient_private.h"

/*******Private Functions*******/
/* Calls receive on the client's socket until either an error happens
 * or the socket is closed. */
static void* read_loop_proc(void* void_client)
{
	TcpClient* client = (TcpClient*)void_client;
	char in_buff[64 * 1024];
	const size_t in_buff_len = 64*1024;
	int in_count;
	int rval;

	flag_raise(&client->flag_pole, THREAD_IS_RUNNING);
	while(client->sock > -1 && client->data_in_cb)
	{
		/* Wait for data. */
		in_count = recv(client->sock, in_buff, in_buff_len, 0);

		/* Disconnected/error occurred. */
		if(in_count <= 0)
		{
			/* We can't call 'TcpClient_disconnect()' if the object is in a delete
			 * state because it has already been called. */
			if(!(client->flag_pole & OBJECT_DELETE_STATE))
				TcpClient_disconnect(client);
			break;
		}
		/* Data received. */
		else
		{
			rval = client->data_in_cb(client, in_buff, in_count);
			if(rval & (SCB_RVAL_CLOSE_CLIENT | SCB_RVAL_STOP_SERVER))
				TcpClient_disconnect(client);
			if(rval & (SCB_RVAL_DELETE))
			{
				pthread_detach(client->read_thread);
				flag_lower(&client->flag_pole, THREAD_CREATED);
				delTcpClient(&client);
			}
		}
	}
	flag_lower(&client->flag_pole, THREAD_IS_RUNNING);

	return(NULL);
}
/*******************************/

/*******Public Functions*******/
/* Connects a client to its host.
 *
 * Returns:
 * 		alib_error
 * 		Anything else is a return value from 'sockopt_cb'. */
int TcpClient_connect(TcpClient* client)
{
	int err;

	if(!client)return(ALIB_BAD_ARG);

	if(client->sock >= 0)return(ALIB_OK);

	/* Create the socket. */
	client->sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(client->sock < 0)
		return(ALIB_FD_ERROR);

	/* Check to see if we need to set any options for the connection's
	 * socket. */
	if(client->sockopt_cb)
	{
		if((err = client->sockopt_cb(client, client->sock)))
			goto f_error;
	}

	/* Connect to the host. */
	err = connect(client->sock, (struct sockaddr*)&client->host_addr,
			sizeof(client->host_addr));
	if(err)
	{
		err = ALIB_TCP_CONNECT_ERR;
		goto f_error;
	}

	/* We are connected, start up reading thread. */
	TcpClient_read_start(client);

	return(ALIB_OK);

f_error:
	close(client->sock);
	client->sock = -1;
	return(err);
}
/* Disconnects the client from its host. */
void TcpClient_disconnect(TcpClient* client)
{
	if(!client)return;

	if(client->sock >= 0)
	{
		close(client->sock);
		client->sock = -1;

		/* Call the disconnect callback. */
		if(client->disconnect_cb)
		{
			int rval = client->disconnect_cb(client);
			if(rval & SCB_RVAL_DELETE)
			{
				delTcpClient(&client);
				return;
			}
		}
	}

	TcpClient_read_stop(client);
}

/* Sends the data to the client's host.
 * This function ensures that all data will be sent.  If not, an error code
 * will be returned.  Will BLOCK until all bytes are transmitted or error occurs.
 *
 * Parameters:
 * 		client: The client who will be sending the data.
 * 		data: The data to be sent.
 * 		data_len: The length of the data to be sent.
 *
 * Returns:
 * 		An alib_error that describes the error. */
alib_error TcpClient_send(TcpClient* client, const char* data, size_t data_len)
{
	int s_count;

	/* Check for argument errors. */
	if(!client || !data)return(ALIB_BAD_ARG);

	/* If the given data length is zero, then we need to
	 * check for the length ourselves, MUST be null terminated. */
	if(data_len == 0)
		data_len = strlen(data);

	/* Ensure we are connected before trying to send. */
	if(client->sock < 0)
		TcpClient_connect(client);

	/* Ensure all data is sent. */
	while(data_len)
	{
		/* Get the number of bytes sent. */
		s_count = send(client->sock, data, data_len, 0);

		/* If it is less than 1, then an error occurred. */
		if(s_count <= 0)
			return(ALIB_TCP_SEND_ERR);

		/* Decrement the number of bytes remaining. */
		data_len -= s_count;
		/* Increment the pointer. */
		data += s_count;
	}

	return(ALIB_OK);
}

/* Starts the reading process on the client. */
void TcpClient_read_start(TcpClient* client)
{
	if(!client)return;

	if(!(client->flag_pole & THREAD_IS_RUNNING) && client->data_in_cb
			&& client->sock >= 0)
	{
		/* Call join to ensure all memory is cleaned up. */
		if(client->flag_pole & THREAD_CREATED)
		{
			pthread_join(client->read_thread, NULL);
			flag_lower(&client->flag_pole, THREAD_CREATED);
		}
		if(!pthread_create(&client->read_thread, NULL, read_loop_proc, client))
			flag_raise(&client->flag_pole, THREAD_CREATED);
	}
}
/* Stops the reading process on the client. */
void TcpClient_read_stop(TcpClient* client)
{
	if(!client)return;

	if(client->flag_pole & THREAD_IS_RUNNING)
	{
		tc_data_in cb = client->data_in_cb;
		client->data_in_cb = NULL;
		pthread_join(client->read_thread, NULL);
		flag_lower(&client->flag_pole, THREAD_CREATED);
		client->data_in_cb = cb;
	}
}

	/* Getters */
/* Returns the user defined extended data of the client.
 *
 * Assumes client is not null.  */
void* TcpClient_get_ex_data(TcpClient* client)
{
	return(client->ex_data);
}
/* Returns 1 if the client is connected, 0 otherwise.
 *
 * Assumes 'client' is not null. */
char TcpClient_is_connected(TcpClient* client)
{
	if(client->sock >= 0)
		return(1);
	else
		return(0);
}
/* Returns the socket for the client.
 *
 * Assumes client is not null. */
int TcpClient_get_socket(TcpClient* client)
{
	return(client->sock);
}
/* Returns 1 if the client is currently reading, 0 otherwise.
 *
 * Assumes 'client' is not null. */
char TcpClient_is_reading(TcpClient* client)
{
	return(client->flag_pole & THREAD_IS_RUNNING);
}

/* Returns the client's ex_data and sets the client's pointer
 * to that data to NULL.  This will not call the free_data_cb.
 *
 * Assumes 'client' is not null. */
void* TcpClient_extract_ex_data(TcpClient* client)
{
	void* rval = client->ex_data;
	client->ex_data = NULL;
	return(rval);
}

/* Returns whether or not the client will close the socket when the object
 * is freed. */
char TcpClient_will_close_sock_on_free(TcpClient* client)
{
    return(client->close_sock_on_free);
}
	/***********/

	/* Setters */
/* Sets the data_in callback function for the client.  If the read
 * thread is not running, it will be started. */
void TcpClient_set_data_in_cb(TcpClient* client, tc_data_in data_in_cb)
{
	if(!client)return;

	client->data_in_cb = data_in_cb;
	if(client->data_in_cb)
		TcpClient_read_start(client);
	else
		TcpClient_read_stop(client);
}
/* Sets the disconnect cb for the client.
 * This callback is called after the client has been disconnected from the host.
 *
 * Assumes 'client' is not null. */
void TcpClient_set_disconnect_cb(TcpClient* client, tc_disconnect disconnect_cb)
{
	client->disconnect_cb = disconnect_cb;
}
/* Sets the sockopt cb for the client.
 * This callback is called whenever the client is about to connect to a host,
 * between the calls to 'socket()' and 'connect()'.
 *
 * Assumes 'client' is not null. */
void TcpClient_set_sockopt_cb(TcpClient* client, tc_sockopt sockopt_cb)
{
	client->sockopt_cb = sockopt_cb;
}

/* Sets the extended data for the client. If extended data already exists,
 * this will try to free the old data before replacing it.
 *
 * Assumes 'client' is not null. */
void TcpClient_set_ex_data(TcpClient* client, void* ex_data,
		alib_free_value free_data_cb)
{
	if(client->free_data_cb && client->ex_data)
		client->free_data_cb(client->ex_data);

	client->ex_data = ex_data;
	client->free_data_cb = free_data_cb;
}

/* Sets whether or not the socket should be closed upon freeing of the object. */
void TcpClient_close_sock_on_free(TcpClient* client, char close_sock_on_free)
{
    client->close_sock_on_free = close_sock_on_free;
}
	/***********/
/******************************/

/*******Constructors*******/
	/* Private Constructors */
/* Base for construction of the object. Used by all other constructors. */
static TcpClient* newTcpClient_base(void* ex_data, alib_free_value free_data_cb)
{
	TcpClient* client = malloc(sizeof(TcpClient));
	if(!client)return(NULL);

	/* Initialize members. */
	client->ex_data = ex_data;
	client->free_data_cb = free_data_cb;
	client->flag_pole = 0;
    client->sock = -1;
    client->close_sock_on_free = 1;
    memset(&client->host_addr, 0, sizeof(client->host_addr));
		/* Callbacks */
	client->disconnect_cb = NULL;
	client->data_in_cb = NULL;
	client->sockopt_cb = NULL;

	return(client);
}
	/************************/

/* Creates a disconnected new TcpClient.
 *
 * Parameters:
 * 		host_addr: The address of the host.
 * 		port: The port of the host.
 * 		ex_data (OPTIONAL): Extended data for the client.
 * 		free_data_cb (OPTIONAL): Used to free the extended data of the client upon
 * 			object destruction.
 *
 * Returns:
 * 		NULL: Error.
 * 		TcpClient*: New TcpClient. */
TcpClient* newTcpClient(const char* host_addr, uint16_t port,
		void* ex_data, alib_free_value free_data_cb)
{
	if(!host_addr)
		return(NULL);

	TcpClient* client = newTcpClient_base(ex_data, free_data_cb);
	//struct hostent* host;

	/* Check for errors with previous level of construction. */
	if(!client)return(NULL);

	/* Initialize members. */
//	host = gethostbyname(host_addr);
//	client->host_addr.sin_addr = *((struct in_addr*)host->h_addr_list);
	client->host_addr.sin_family = AF_INET;
	client->host_addr.sin_addr.s_addr = inet_addr(host_addr);
	client->host_addr.sin_port = htons(port);

	return(client);
}
/* Creates a TcpClient from an already connected socket.
 *
 * Parameters:
 * 		sock: The socket to build the client from.
 *      close_sock_on_free: If !0, the socket will be closed upon object deletio
n.
 * 		ex_data (OPTIONAL): Extended data for the client.
 * 		free_data_cb (OPTIONAL): Used to free the extended data of the client upon
 * 			object destruction.
 *
 * Returns:
 * 		NULL: Error.
 * 		TcpClient*: New TcpClient.*/
TcpClient* newTcpClient_from_socket(int sock, char close_sock_on_free, void* ex_data, alib_free_value free_data_cb)
{
	if(sock < 0)return(NULL);

    socklen_t addr_len;
	TcpClient* client = newTcpClient_base(ex_data, free_data_cb);
	if(!client)return(NULL);

	/* Initialize members. */
	client->sock = sock;
    client->close_sock_on_free  = close_sock_on_free;

	/* Get the host address from the socket. If the function fails, then
	 * we should simply delete the object. */
    addr_len = sizeof(client->host_addr);
	if(getsockname(sock, (struct sockaddr*)&client->host_addr, &addr_len))
		delTcpClient(&client);

	return(client);
}

/* Disconnects the client and destroys the object. */
void delTcpClient(TcpClient** client)
{
	if(!client || !*client ||
			((*client)->flag_pole & OBJECT_DELETE_STATE))return;

	/* Place the object in a delete state. */
	flag_raise(&(*client)->flag_pole, OBJECT_DELETE_STATE);

    if((*client)->close_sock_on_free)
    	TcpClient_disconnect(*client);
    else
        TcpClient_read_stop(*client);

	if((*client)->free_data_cb)
		(*client)->free_data_cb((*client)->ex_data);

	free(*client);
	client = NULL;
}
/**************************/
