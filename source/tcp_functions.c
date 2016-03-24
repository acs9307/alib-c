#include "tcp_functions.h"

/* Sends the data to a TCP host and then closes the connection upon return.
 * Only suggest if a one time message needs to be sent, otherwise use a better
 * client such as UvTcpClient.
 *
 * This function WILL block until a connection is made or a connection fails.
 *
 * Parameters:
 * 		host_address: The address of the host, either IP or DNS.
 * 		port: The port of the host application.
 * 		data: The data to be sent to the host.  If 0, strlen() will be called on 'data'.
 * 		data_len: The length of the 'data' to be sent to the host.
 */
alib_error tcp_send_data_by_host(const char* host_address, uint16_t port,
		const char* data, size_t data_len)
{
	int sock = -1, err = 0;
	struct sockaddr_in host_addr;
	size_t send_count;
	struct hostent* host;

	/* Allocate the socket. */
	sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sock < 0)
		return(ALIB_FD_ERR);

	/* Initialize the host address struct. */
	memset(&host_addr, 0, sizeof(host_addr));
	host = gethostbyname(host_address);
	if(host)
	{
		host_addr.sin_family = AF_INET;
		host_addr.sin_port = htons(port);
		host_addr.sin_addr =  *((struct in_addr*)*host->h_addr_list);
	}

	/* Connect. */
	err = connect(sock, (struct sockaddr*)&host_addr, sizeof(host_addr));
	if(err)return(ALIB_TCP_CONNECT_ERR);

	/* Ensure all the data is sent. */
	if(!data_len)
		data_len = strlen(data);
	while(data_len)
	{
		send_count = send(sock, data, data_len, 0);
		if(send_count < 0)
			return(ALIB_TCP_SEND_ERR);
		else
			data_len -= send_count;
	}

	fsync(sock);
	close(sock);
	return(ALIB_OK);
}
