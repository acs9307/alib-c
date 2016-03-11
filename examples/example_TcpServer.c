#include <stdio.h>

#include "TcpServer.h"

static server_cb_rval client_connected(TcpServer* server, socket_package* client)
{
	printf("Client connected on socket: %d\n", client->sock);

	return(SCB_RVAL_STOP_SERVER);
	return(SCB_RVAL_DEFAULT);
}
static server_cb_rval client_data_ready(TcpServer* server, socket_package* client,
		void** in_buff, int* buff_len)
{
	printf("Data ready on client socket!\n");
	*buff_len = recv(client->sock, *in_buff, DEFAULT_INPUT_BUFF_SIZE, 0);
	return(SCB_RVAL_DEFAULT);
}
static server_cb_rval client_data_in(TcpServer* server, socket_package* client,
		const void* in_buff,	size_t buff_len)
{
	printf("Client data received!\n");
	printf("\trecv: ");fwrite(in_buff, 1, buff_len, stdout);printf("\n");

	printf("send %d\n", (int)send(client->sock, in_buff, buff_len, 0));

	return(SCB_RVAL_DEFAULT);
}
static server_cb_rval client_disconnected(TcpServer* server, socket_package* client)
{
	printf("Client on socket %d disconnected!\n", client->sock);
	return(SCB_RVAL_DEFAULT);
}

int main()
{
	TcpServer* server = newTcpServer(5555, NULL, NULL);
	printf("newTcpServer(): %s\n", (server)?"Succeeded!":"Failed!");
	if(!server)return(1);

	TcpServer_set_client_connected_cb(server, client_connected);
	TcpServer_set_client_data_ready_cb(server, client_data_ready);
	TcpServer_set_client_data_in_cb(server, client_data_in);
	TcpServer_set_client_disconnected_cb(server, client_disconnected);

	printf("TcpServer_start(): %d\n", TcpServer_start(server));

//	printf("TcpServer_start_async(): %d\n", TcpServer_start_async(server));
//	while(!TcpServer_is_running(server))sleep(1);
//	TcpServer_wait_for_thread_return(server);

	delTcpServer(&server);
	printf("Application Closing!\n");
	return(0);
}

