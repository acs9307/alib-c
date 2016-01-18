#include "FdClient.h"

#include <unistd.h>
#include <stdio.h>

static void disconnect_cb(FdClient* client)
{
	printf("Client disconnected!\n");
}
static void fd_received(FdClient* client, int fd)
{
	printf("fd: %d\n", fd);
	if(fd > -1)
		close(fd);
}

int main()
{
	FdClient* client = newFdClient("/tmp/2dhuo_SoftwareManager", 0);

	printf("Starting listen_async()!\n");
	printf("FdClient_listen: %d\n", FdClient_listen_async(client, fd_received, disconnect_cb));

	printf("Client is listening!\n");
	{
		size_t timeout = 10;  //seconds
		while(FdClient_is_connected(client) && --timeout)
			sleep(1);

		if(!timeout)
		{
			printf("Timeout has been reached, client closing.\n");
			FdClient_close(client);
		}
	}

	delFdClient(&client);
	printf("Application closing!\n");
	return(0);
}
