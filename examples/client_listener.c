#include <stdio.h>
#include "FdServer.h"
#include "ClientListener.h"
#include "FdClient.h"
#include "TcpServer.h"

/********ClientListener/FdServer*******/
static server_cb_rval fd_received(FdServer* server, fds_package* pack, int sock)
{
	ClientListener* listener = (ClientListener*)FdServer_get_extended_data(server);

	printf("ClientListener_add: %d\n", ClientListener_add(listener, sock, 1, listener, NULL));

	return(SCB_RVAL_DEFAULT);
}
static void fd_disconnected(FdServer* server, fds_package* pack)
{
	printf("Lost connection to FdClient!\n");
	FdServer_stop(server);
}

static server_cb_rval listener_data_ready(ClientListener* listener, socket_package* client,
		char** data, long* data_len)
{
	printf("'data_ready()' callback called!\n");
	*data_len = recv(client->sock, *data, DEFAULT_INPUT_BUFF_SIZE, 0);
	return(SCB_RVAL_DEFAULT);
}
static server_cb_rval listener_data_in(ClientListener* listener, socket_package* client,
		char* data, size_t data_len)
{
	printf("Received '");
		fwrite(data, 1, data_len, stdout);
		printf("' from client on socket %d.\n", client->sock);

	return(SCB_RVAL_DEFAULT);
}
static server_cb_rval listener_disconnected(ClientListener* listener, socket_package* client)
{
	printf("Client on socket %d disconnected!\n", client->sock);

	return(SCB_RVAL_DEFAULT);
}
static void client_list_empty(ClientListener* listener)
{
	printf("Client list is empty!\n");
}

static void FdServer_proc()
{
	FdServer* server;
	ClientListener* listener = newClientListener(NULL, NULL);
	ClientListener_set_client_data_ready_cb(listener, listener_data_ready);
	ClientListener_set_client_data_in_cb(listener, listener_data_in);
	ClientListener_set_client_disconnected_cb(listener, listener_disconnected);
	ClientListener_set_client_list_emtpy_cb(listener, client_list_empty);

	remove("/tmp/001");
	server = newFdServer("/tmp/001", 0, NULL, fd_received);
	FdServer_set_on_disconnect_cb(server, fd_disconnected);
	FdServer_set_extended_data(server, listener, NULL, 0);
	FdServer_run(server);

	delClientListener(&listener);
	delFdServer(&server);
}
/**************************************/

/*******TcpServer/FdClient*******/
static server_cb_rval client_connected(TcpServer* server, socket_package* client)
{
	int err;
	FdClient* fd_client = (FdClient*)TcpServer_get_extended_data(server);

	if((err = FdClient_send(fd_client, client->sock)))
	{
		printf("FdClient_send failed with %d.\n", err);
		return(SCB_RVAL_STOP_SERVER);
	}
	else
		return(SCB_RVAL_CLOSE_CLIENT);
}

static void FdClient_proc()
{
	FdClient* client = newFdClient("/tmp/001", 0);
	TcpServer* server;

	FdClient_connect(client);

	server = newTcpServer(7777, client, NULL);
	TcpServer_set_client_connected_cb(server, client_connected);

	TcpServer_start(server);

	delTcpServer(&server);
	delFdClient(&client);
}
/********************************/

int main()
{

	if(fork())
		FdServer_proc();
	else
	{
		sleep(1);
		FdClient_proc();
	}

	return(0);
}
