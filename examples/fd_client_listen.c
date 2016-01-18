#include "FdServer.h"
#include "FdClient.h"
#include "TcpClient.h"
#include "event_server.h"
#include "ancillary.h"
#include "tcp_functions.h"

#include <unistd.h>

static char stop_sending = 0;
static char* send_msg = "hello world";

/* Simple example showing how to send sockets from one process to another.
 *
 * I split everything up into separate processes as it is key to the example, however
 * you should never let FdServer run on it's own process as it will create a zombie
 * process if it is not cleaned up before the main process closes.
 *
 * The server is created as the */

/*******FdServer*******/
server_cb_rval on_connect(FdServer* server, fds_package* package)
{
	TcpClient* client = newTcpClient("127.0.0.1", 8888, NULL, NULL);

	printf("client connected to fd_server!\n");
	printf("TcpClient_connect: %d\n", TcpClient_connect(client));
	usleep(1000);

	printf("ancil_send_fd: %d\n", ancil_send_fd(package->sock, TcpClient_get_socket(client)));
	TcpClient_disconnect(client);

	return(SCB_RVAL_DEFAULT);
}

void proc_receive()
{
	FdServer* receiver = newFdServer("/tmp/003", 0, on_connect, NULL);

	/* Synchronous */
//	FdServer_run(receiver);
	/***************/

	/* Async */
	FdServer_run_on_thread(receiver);
	getchar();
	FdServer_stop(receiver);
	/*********/

	printf("Preparing to delete the receiver!\n");
	delFdServer(&receiver);
}
/************************/

/*******FdClient*******/
void fd_client_received(FdClient* client, int sock)
{
	/* Send the server the message then close the socket. */
	send(sock, send_msg, strlen(send_msg), 0);
	close(sock);

	printf("fd_client_received: %d\n", sock);
	FdClient_close(client);
}

void fd_client_proc()
{
	FdClient* sender = newFdClient("/tmp/003", 0);
	int err;

	/* Ensure we are connected. */
	sleep(1);
	if((err = FdClient_connect(sender)))
	{
		printf("Could not connect to FdServer...\n");
		stop_sending = 1;
		goto f_return;
	}
	printf("FdClient_connect: %d\n", err);

	printf("FdClient: %d\n", FdClient_get_sock(sender));

	printf("FdClient_listen: %d\n", FdClient_listen(sender, fd_client_received));
	printf("\terrno: %d\n", errno);

f_return:
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
	int pid;

	pid = fork();

	/* Run the TCP server on a child process. */
	if(pid == 0)
	{
		event_server* server;
		printf("new_event_server: %d\n", new_event_server(&server, 8888, NULL, data_in_cb, NULL, NULL, NULL, NULL, 1));
		printf("Event server starting!\n");
		event_server_start(server);
		del_event_server(&server);
		printf("Event server proc complete!\n");
		return(0);
	}

	/* The temp file cannot exist, if it does then the bind for
	 * FdReciever will fail. */
	system("rm /tmp/003");
	pid = fork();

	/* Run an FD client on a child process. */
	if(pid == 0)
	{
		sleep(1);
		printf("Send process starting!\n");
		while(!stop_sending)
			fd_client_proc();

		printf("tcp_send_data_by_host: %d\n", tcp_send_data_by_host("127.0.0.1", 8888, ".quit", 0));
		printf("Send proc complete!\n");
		return(0);
	}

	/* Run the FD server on the main process. */
	printf("Receive process starting!\n");
	proc_receive();
	printf("Receive proc finished!\n");
	return(0);
}
