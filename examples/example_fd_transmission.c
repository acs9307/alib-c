#include "FdServer.h"
#include "FdClient.h"
#include "TcpClient.h"
#include "event_server.h"

#include <unistd.h>

static char stop_sending = 0;
static char* send_msg = "hello world";

/* Simple example showing how to send sockets from one process to another.
 *
 * I split everything up into separate processes as it is key to the example, however
 * you should never let FdServer run on it's own process as it will create a zombie
 * process if it is not cleaned up before the main process closes. */

/*******FdServer*******/
server_cb_rval on_receive(FdServer* receiver, fds_package* package, int sock)
{
	/* Check to see if we need to start closing things down. */
	if(strcmp("hello world", send_msg) != 0)
		stop_sending = 1;

	/* Send the server the message then close the socket. */
	send(sock, send_msg, strlen(send_msg), 0);
	close(sock);

	/* Check if we just need to close the client or if we need
	 * to stop the server. */
	if(!stop_sending)
		return(SCB_RVAL_CLOSE_CLIENT);
	else
		return(SCB_RVAL_STOP_SERVER);
}

void proc_receive()
{
	FdServer* receiver = newFdServer("/tmp/003", 0, NULL, on_receive);

	/* Single threaded. */
//	FdServer_run(receiver);

	/* Threaded. */
	FdServer_run_on_thread(receiver);
	getchar();
	send_msg = ".quit";
	while(FdServer_is_running(receiver));

	printf("Preparing to delete the receiver!\n");
	delFdServer(&receiver);
}
/************************/

/*******FdClient*******/
void proc_send()
{
	TcpClient* client = newTcpClient("127.0.0.1", 8888, NULL, NULL);
	FdClient* sender = newFdClient("/tmp/003", 0);
	int err;

	TcpClient_connect(client);
	FdClient_connect(sender);

	printf("TcpClient: %d\n", TcpClient_get_socket(client));
	printf("FdClient: %d\n", FdClient_get_sock(sender));

	if((err = FdClient_send(sender, TcpClient_get_socket(client))) ||
			FdClient_get_sock(sender) < 0 || TcpClient_get_socket(client) < 0)
		stop_sending = 1;

	delTcpClient(&client);
	delFdClient(&sender);
}
/**********************/

/*******Server*******/
static escb_rval data_in_cb(const char* data, size_t data_len, void* user_data)
{
	printf("Received: ");
	fwrite(data, 1, data_len, stdout);
	printf("\n");

	if(!stop_sending)
		return(ESCB_CLIENT_CLOSE);
	else
		return(ESCB_CLIENT_CLOSE | ESCB_SERVER_SHUTDOWN);
}
/********************/

int main()
{
	int fd;
	event_server* server;

	printf("new_event_server: %d\n", new_event_server(&server, 8888, NULL, data_in_cb, NULL, NULL, NULL, NULL, 1));

	fd = fork();
	if(fd == 0)
	{
		printf("Event server starting!\n");
		event_server_start(server);
		del_event_server(&server);
		return(0);
	}

	/* The temp file cannot exist, if it does then the bind for
	 * FdReciever will fail. */
	system("rm /tmp/003");
	fd = fork();

	if(fd != 0)
	{
		printf("Receive process starting!\n");
		proc_receive();
	}
	else
	{
		printf("Send process starting!\n");
		while(!stop_sending)
		{
			proc_send();
			
			/* Add to sleep otherwise a ton of connections will be created then closed 
			 * and the fd transmitter processes will stop running, not sure why. */
			sleep(1);
		}
	}

	return(0);
}
