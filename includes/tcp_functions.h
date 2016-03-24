#ifndef TCP_FUNCTIONS_IS_DEFINED
#define TCP_FUNCTIONS_IS_DEFINED

#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/types.h>
#include <inttypes.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>

#include "alib_error.h"

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
		const char* data, size_t data_len);

#endif
