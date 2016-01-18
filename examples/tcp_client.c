#include "TcpClient.h"
#include "TcpServer.h"

server_cb_rval data_in(TcpClient* client, const char* data, size_t data_len)
{
	printf("Received: ");
		fwrite(data, 1, data_len, stdout);
		printf("\n");
	return(SCB_RVAL_DELETE);
}
server_cb_rval disconnect(TcpClient* client)
{
	printf("Client disconnected from server!\n");
	return(SCB_RVAL_DEFAULT);
}

void client_proc()
{
	TcpClient* client = newTcpClient("127.0.0.1", 7777, NULL, NULL);

	printf("TcpClient_connect(): %d\n", TcpClient_connect(client));
	TcpClient_set_data_in_cb(client, data_in);
	TcpClient_set_disconnect_cb(client, disconnect);

	TcpClient_send(client, "hello there", 0);
}

server_cb_rval reply_proc(TcpServer* server, socket_package* client, char* data,
		size_t data_len)
{
	send(client->sock, data, data_len, 0);
	return(SCB_RVAL_DEFAULT);
}
server_cb_rval client_disconnected(TcpServer* server, socket_package* client)
{
	return(SCB_RVAL_STOP_SERVER);
}
void server_proc()
{
	TcpServer* server = newTcpServer(7777, NULL, NULL);

	TcpServer_set_client_data_in_cb(server, reply_proc);
	TcpServer_set_client_disconnected_cb(server, client_disconnected);
	TcpServer_start(server);

	delTcpServer(&server);
}

int main()
{
	if(fork())
	{
		sleep(1);
		client_proc();
	}
	else
		server_proc();

	printf("Application returning!\n");
	return(0);
}
