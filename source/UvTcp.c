#include "UvTcp.h"

#include <stdio.h>

/*******Private Structs*******/
typedef struct write_package
{
	void* data;
	uv_write_cb write_cb;
	char* buff;
}write_package;
/*****************************/

/*******Private Functions*******/
/* Called after the write has been made.  Designed to automatically free
 * the data allocated for the write and call the user defined 'write_cb'
 * as needed. */
static void default_write_cb(uv_write_t* write_req, int status)
{
	write_package* package = write_req->data;
	if(package->write_cb)
		package->write_cb(write_req, status);

	/* For some reason the idiots that designed this library can't seem to keep
	 * anything standardized nor can they document worth a shit.  Somewhere
	 * along the way the 'write->req->buf->base' gets freed.  If freed here,
	 * segment fault will occur. */
	if(package)
	{
		if(package->buff)
			free(package->buff);
		free(package);
	}
	free(write_req);
}
/*******************************/

/*******Public Functions*******/
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
		uv_write_cb write_cb)
{
	uv_buf_t uv_buff;
	uv_write_t* write_req = NULL;
	write_package* package = NULL;

	/* Check for argument error. */
	if(!client || !buff || !*buff)
		return(ALIB_BAD_ARG);

	/* Calculate the size if the given buff len is 0. s*/
	if(buff_len == 0)
		*((size_t*)&buff_len) = strlen(*buff);

	/* Allocate memory for the new buffer.
	 * This should be freed on the write callback. */
	uv_buff = uv_buf_init((char*)(*buff), buff_len);

	/* Allocate the other objects. */
	write_req = malloc(sizeof(uv_write_t));
	package = malloc(sizeof(package));
	if(!write_req || !package)goto f_error;

	/* Initialize the write package.
	 * This should be freed after the write callback is made. */
	package->data = client->data;
	package->write_cb = write_cb;
	package->buff = (char*)(*buff);

	write_req->data = package;
	uv_write(write_req, client, &uv_buff, 1, default_write_cb);

	*buff = NULL;
	return(ALIB_OK);
f_error:
	if(write_req)
		free(write_req);
	if(package)
		free(package);

	return(ALIB_MEM_ERR);
}
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
alib_error UvTcp_write(uv_stream_t* client, const char* buff, size_t buff_len,
		uv_write_cb write_cb)
{
	char* b;
	int err;

	if(!buff_len)
		buff_len = strlen(buff);

	if(!strncpy_alloc(&b, buff, buff_len))
		return(ALIB_MEM_ERR);

	err = UvTcp_write_free(client, &b, buff_len, write_cb);

	if(b)free((char*)b);

	return(err);
}

/* Creates a 'uv_tcp_t' object for the socket and adds it to the given loop.
 *
 * Parameters:
 * 		loop: (REQUIRED) The loop to add the client to.
 * 		sock: (REQUIRED) The socket of the client.
 * 		out_client_handle: (OPTIONAL) A double pointer of a 'uv_tcp_t' handle.
 * 			If NULL, then it will not be set to a value.
 * 			If the pointer is already pointing to another uv_tcp_t object, the
 * 			object will not be modified, however, the pointer will be modified.  */
alib_error UvTcp_add_socket_to_loop(uv_loop_t* loop, int sock, uv_tcp_t** out_client_handle)
{
	uv_tcp_t* handle;
	alib_error err;

	if(sock < 0)return(ALIB_BAD_ARG);

	/* Allocate needed memory. */
	handle = malloc(sizeof(uv_tcp_t));
	if(!handle)return(ALIB_MEM_ERR);

	/* Initialize the client handle. */
	if(uv_tcp_init(loop, handle) ||
			uv_tcp_open(handle, sock))
	{
		err = ALIB_FD_ERR;
		goto f_error;
	}

	/* Start running in the loop. */
	uv_run(handle->loop, UV_RUN_NOWAIT);

	/* Check to see if a handle pointer was given. */
	if(out_client_handle)
		*out_client_handle = handle;

	return(ALIB_OK);
f_error:
	if(handle)
		free(handle);
	return(err);
}

/* Allocates a block of memory the size of the suggested size.
 * This conforms to the uv_alloc_cb function pointer prototype.
 * Because memory is allocated here, the memory must be freed in
 * the uv_read_cb callback.
 *
 * This is a convenience callback function and, in most cases, unsuitable
 * for end application implementation. */
void UvTcp_default_alloc_cb(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf)
{
	buf->len = suggested_size;
	buf->base = malloc(buf->len);
}
/******************************/
