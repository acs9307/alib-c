#include "TcpClient.h"
#include "TcpServer.h"
#include "alib_string.h"

/*******Client*******/
server_cb_rval data_in(TcpClient* client, const char* data, size_t data_len)
{
	printf("Received: ");
		fwrite(data, 1, data_len, stdout);
		printf("\n");
	return(SCB_RVAL_DEFAULT);
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

#if 1
	while(1)
	{
		printf("Waiting for input.\n");
		char c = getchar();
		char buff[64 * 1024] = {0};
		size_t read_count = 0;
		for(; c != '\n' && read_count < sizeof(buff); ++read_count, c = getchar())
			buff[read_count] = c;

		if(str_match(buff, strlen(buff), ".quit", 5) == 0)
		{
			printf("Shutting down!\n");
			goto f_return;
		}
		else if(str_match(buff, strlen(buff), ".stopReading", strlen(".stopReading")) == 0)
		{
			printf("Turning off reading.\n");
			TcpClient_read_stop(client);
			printf("Reading stopped!\n");
		}
		else if(str_match(buff, strlen(buff), ".startReading", strlen(".startReading")) == 0)
		{
			printf("Turning on reading.\n");
			TcpClient_read_start(client);
			printf("Reading started!\n");
		}
		else
		{
			printf("Sending (%d bytes): %s\n", (int)read_count, buff);
			printf("TcpClient_send(): %d\n", TcpClient_send(client, buff, read_count));
		}
	}
#else
	TcpClient_read_stop(client);
#endif

f_return:
	delTcpClient(&client);
}
/********************/

/*******Server*******/
server_cb_rval reply_proc(TcpServer* server, socket_package* client, char* data,
		size_t data_len)
{
	printf("reply proc: %d\n", send(client->sock, data, data_len, 0));
	return(SCB_RVAL_DEFAULT);
}
server_cb_rval client_disconnected(TcpServer* server, socket_package* client)
{
	return(SCB_RVAL_STOP_SERVER);
}
server_cb_rval client_connected(TcpServer* server, socket_package* client)
{
	return(SCB_RVAL_CLOSE_CLIENT);
}
void server_proc()
{
	TcpServer* server = newTcpServer(7777, NULL, NULL);

	TcpServer_set_client_data_in_cb(server, reply_proc);
	TcpServer_set_client_disconnected_cb(server, client_disconnected);
	//TcpServer_set_client_connected_cb(server, client_connected);
	TcpServer_start(server);

	delTcpServer(&server);
}
/********************/

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
