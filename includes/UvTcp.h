#ifndef UV_TCP_IS_DEFINED
#define UV_TCP_IS_DEFINED

#include <uv.h>
#include "alib_string.h"
#include "alib_error.h"

/* Writes data to a client.  Data provided will be freed after write has been completed.
 * Any data that should be passed to the 'write_cb' should be set in the client's data field.
 * Any memory allocated by the function will be freed internally.
 *
 * Parameters:
 * 		client: A handle to the client.
 * 		buff: A double pointer to the buffer to be written to the client.
 * 			On failure, this will not be freed.  If it has been freed, the pointer
 * 			will be set to NULL.
 * 		buff_len: The length of the buffer in bytes.
 * 		write_cb: (OPTIONAL) This is a callback called when the write completes.
 *
 * Returns:
 * 		ALIB_OK: Successfully written.
 * 		!ALIB_OK: Error occurred, no data was written.
 */
alib_error UvTcp_write_free(uv_stream_t* client, char** buff, const size_t buff_len,
		uv_write_cb write_cb);

/* Writes data to a client.  Data provided will be copied and therefore internally freed.
 * Any data that should be passed to the 'write_cb' should be set in the client's data field.
 * Any memory allocated by the function will be freed internally, but any memory allocated outside must be
 * freed by the user.
 *
 * Parameters:
 * 		client: A handle to the client.
 * 		buff: The buffer to be written to the client.
 * 		buff_len: The length of the buffer in bytes.
 * 		write_cb: (OPTIONAL) This is a callback called when the write completes.
 *
 * Returns:
 * 		ALIB_OK: Successfully written.
 * 		!ALIB_OK: Error occurred, no data was written.
 */
alib_error UvTcp_write(uv_stream_t* client, const char* buff, const size_t buff_len,
		uv_write_cb write_cb);

/* Creates a 'uv_tcp_t' object for the socket and adds it to the given loop.
*
* Parameters:
* 		loop: (REQUIRED) The loop to add the client to.
* 		sock: (REQUIRED) The socket of the client.
* 		out_client_handle: (OPTIONAL) A double pointer of a 'uv_tcp_t' handle.
* 			If NULL, then it will not be set to a value.
* 			If the pointer is already pointing to another uv_tcp_t object, the
* 			object will not be modified, however, the pointer will be modified.  */
alib_error UvTcp_add_socket_to_loop(uv_loop_t* loop, int sock,
		uv_tcp_t** out_client_handle);
/* Allocates a block of memory the size of the suggested size.
 * This conforms to the uv_alloc_cb function pointer prototype.
 * Because memory is allocated here, the memory must be freed in
 * the uv_read_cb callback.
 *
 * This is a convenience callback function and, in most cases, unsuitable
 * for end application implementation. */
void UvTcp_default_alloc_cb(uv_handle_t* handle, size_t suggested_size,
		uv_buf_t* buf);

#endif
