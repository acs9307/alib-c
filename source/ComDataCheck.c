#include "ComDataCheck_private.h"

/*******Public Functions*******/
/* Checks the state of the ComDataCheck object and returns
 * a related ComDataCheck_RVal. */
int ComDataCheck_check(ComDataCheck* cdc)
{
	if(!cdc)return(ALIB_BAD_ARG);

	if(cdc->input)
	{
		size_t buff_len = BinaryBuffer_get_length(cdc->buff);
		if(buff_len < cdc->data_len)
			return(CDC_WAITING);
		else if(buff_len == cdc->data_len)
			return(CDC_COMPLETE);
		else
		{
			BinaryBuffer_append(cdc->unread_buff, BinaryBuffer_get_raw_buff(cdc->buff) + cdc->data_len, 
				buff_len - cdc->data_len);
			BinaryBuffer_remove(cdc->buff, cdc->data_len, buff_len);
			return(CDC_DATA_REMAINING);
		}
	}
	else
		return(CDC_COMPLETE);
}

/* Processes the unprocessed data in the ComDataCheck object by calling 
 * 'ComDataCheck_append()' to append the unprocessed data onto the end of the object's buffer.
 * 
 * Returns the same value as 'ComDataCheck_append()'. */
int ComDataCheck_process_buffered_data(ComDataCheck* cdc)
{
	if(!cdc)return(ALIB_BAD_ARG);

	size_t buff_len = BinaryBuffer_get_length(cdc->unread_buff);
	unsigned char* buff = BinaryBuffer_extract_buffer(cdc->unread_buff);
	return(ComDataCheck_append(cdc, buff, buff_len));
}

/* Appends data to a ComDataCheck object and returns
 * a ComDataCheck_RVal or error code for the object.
 *
 * ComDataCheck_append() has two behaviors, one for each mode
 * of the object.
 *
 * 		Input: Appends the incoming data and fills the object's members
 * 			as needed.  Returns a ComDataCheck_RVal describing the state
 * 			of the object upon processing completion.  No matter what
 * 			ComDataCheck_RVal is returned, the data will still be appended
 * 			to the object.
 * 		Output: Simply appends data to the object and updates internal
 * 			members.  Returns CDC_COMPLETE on success.
 *
 * Parameters:
 * 		cdc: The object to modify.
 * 		data: The data to be appended.
 * 		data_len: The length of 'data' in bytes.
 *
 * Returns:
 *		alib_error: Error occurred.
 *		ComDataCheck_RVal: Describes the state of the object. */
int ComDataCheck_append(ComDataCheck* cdc, unsigned char* data, size_t data_len)
{
	int32_t err;

	/* Check for argument errors. */
	if(!cdc)return(ALIB_BAD_ARG);
	
	if(BinaryBuffer_get_length(cdc->unread_buff))
	{
		err = ComDataCheck_process_buffered_data(cdc);
		if(err == CDC_COMPLETE || err == CDC_DATA_REMAINING)
		{
			BinaryBuffer_append(cdc->unread_buff, data, data_len);
			return(err);
		}
	}
	
	if(!data || !data_len)
		return(ComDataCheck_check(cdc));

	/* If the pack is used for input. */
	if(cdc->input)
	{
		/* Check to see if the data length member has been set. */
		if(!cdc->data_len)
		{
			/* If the calculated length is less than the data length,
			 * then we have a completed header. */
			if(sizeof(cdc->data_len) < data_len + BinaryBuffer_get_length(cdc->buff))
			{
				/* Some of the header exists in the buffer.  Therefore we need
				 * to put the data together and parse the header before
				 * appending data to the buffer. */
				if(BinaryBuffer_get_length(cdc->buff))
				{
					/* Store the number of bytes remaining for the header. */
					char data_len_diff = sizeof(cdc->data_len) -
							BinaryBuffer_get_length(cdc->buff);

					/* Finish appending the header. */
					err = BinaryBuffer_append(cdc->buff, data, data_len_diff);
					if(err)return(err);

					/* Parse the header and clear the buffer. */
					cdc->data_len = ntohl(*((int32_t*)BinaryBuffer_get_raw_buff(cdc->buff)));
					data += data_len_diff;
					data_len -= data_len_diff;
					BinaryBuffer_clear_no_resize(cdc->buff);

				}
				/* If there is nothing in the buffer, then we simply need to parse the
				 * header of the given data array. */
				else
				{
					cdc->data_len = ntohl(*((int32_t*)data));
					data += sizeof(cdc->data_len);
					data_len -= sizeof(cdc->data_len);
				}

				/* Append data that is after header. */
				err = BinaryBuffer_append(cdc->buff, data,
						data_len);
				if(err)return(err);
			}
			/* The full header has not been received yet. Append
			 * data to the beginning of the buffer.  The buffer
			 * will be used to temporarily store the header until we get the entire
			 * header, in which case we will parse the data and remove it from the
			 * buffer. */
			else
			{
				err = BinaryBuffer_append(cdc->buff, data, data_len);
				if(err)
					return(err);
				else
					return(CDC_WAITING);
			}
		}
		/* Data length has been received, now we simply add to the buffer. */
		else
		{
			err = BinaryBuffer_append(cdc->buff, data, data_len);
			if(err)return(err);
		}

		/* Return the status of the package. */
		return(ComDataCheck_check(cdc));
	}
	/* The pack is used for output.
	 * Simply append the data to the buffer and update the
	 * data length members of the object. */
	else
	{
		err = BinaryBuffer_append(cdc->buff, data, data_len);
		if(err)return(err);

		if(BinaryBuffer_get_length(cdc->buff) > INT32_MAX)
			return(ALIB_INTERNAL_MAX_REACHED);
		else
			cdc->data_len = BinaryBuffer_get_length(cdc->buff);

		return(CDC_COMPLETE);
	}
}

/* Clears any data stored in the ComDataCheck object. */
void ComDataCheck_clear(ComDataCheck* cdc)
{
	if(!cdc)return;

	cdc->data_len = 0;
	BinaryBuffer_clear(cdc->buff);
}

/* Sends the data in the given ComDataCheck to the given 'sock'.
 *
 * Parameters:
 * 		cdc: The object containing the data to send.
 * 		sock: The socket of the node to send data to.
 * 		flags: The flags to use with 'send()'.
 */
alib_error ComDataCheck_send(ComDataCheck* cdc, int sock, int flags)
{
	if(!cdc || sock <= 0)return(ALIB_BAD_ARG);

	int32_t len_net = BinaryBuffer_get_length(cdc->buff);
    len_net = htonl(len_net);
	if(send(sock, &len_net,	sizeof(cdc->data_len), flags) != sizeof(cdc->data_len))
	{
		return(ALIB_FILE_WRITE_ERR);
	}

	if(send(sock, BinaryBuffer_get_raw_buff(cdc->buff),
			BinaryBuffer_get_length(cdc->buff), flags) != BinaryBuffer_get_length(cdc->buff))
	{
		return(ALIB_FILE_WRITE_ERR);
	}

	return(ALIB_OK);
}

/* Receives data on a given socket and places said data into the provided
 * ComDataCheck object.
 *
 * THIS FUNCTION WILL BLOCK until all the data received completes
 * the pack or an error occurs.
 *
 * Uses a buffer of size 'DEFAULT_INPUT_BUFF_SIZE'.
 *
 * Parameters:
 * 		cdc: The ComDataCheck to store received data from.
 * 		sock: The socket to receive data from.
 * 		flags: The flags to use with 'recv()'. */
alib_error ComDataCheck_recv(ComDataCheck* cdc, int sock, int flags)
{
	if(!cdc || sock <= 0 || !cdc->input)return(ALIB_BAD_ARG);

	int rval;
	unsigned char buff[DEFAULT_INPUT_BUFF_SIZE];
	size_t recv_len;

	/* Continue until we have read all data. */
	do
	{
		recv_len = recv(sock, buff, DEFAULT_INPUT_BUFF_SIZE, flags);
		if(recv_len <= 0)
			break;
	}while((rval = ComDataCheck_append(cdc, buff, recv_len)) == CDC_WAITING);

	return(rval);
}
/* Same as 'ComDataCheck_recv()' except with a timeout.
 * This will set the receive timeout option for the socket.  If a timeout is
 * not desired after this call, then the user MUST manually reset the timeout for the socket.
 *
 * Parameters:
 * 		cdc: The ComDataCheck to store received data from.
 * 		sock: The socket to receive data from.
 * 		flags: The flags to use with 'recv()'.
 * 		secs: The number of seconds to wait before timing out. (Added to 'micros')
 * 		micros: The number of micro seconds to wait before timing out. (Added to 'secs') */
alib_error ComDataCheck_recv_timeout(ComDataCheck* cdc, int sock, int flags,
		size_t secs, size_t micros)
{
	if(set_sock_recv_timeout(sock, secs, micros))
		return(ALIB_FILE_ERR);

	alib_error err = ComDataCheck_recv(cdc, sock, flags);
	if(err == CDC_WAITING)
		return(ALIB_TIMEOUT);
	else
		return(err);
}

	/* Getters */
/* Returns the buffer of the ComDataCheck object.
 *
 * Assumes 'cdc' is not null. */
BinaryBuffer* ComDataCheck_get_buffer (ComDataCheck* cdc)
{
	return(cdc->buff);
}
/* Returns the expected length of the data portion of the object.
 *
 * Assumes 'cdc' is not null. */
int ComDataCheck_get_expected_len(ComDataCheck* cdc)
{
	return(cdc->data_len);
}

void* ComDataCheck_get_extended_data(ComDataCheck* cdc){return(cdc->extended_data);}
	/***********/

	/* Setters */
/* Sets the mode of the ComDataCheck.  If the mode is changed,
 * the 'data_len' member will be updated accordingly, however
 * data will not be cleared.  To clear data, manually call
 * ComDataCheck_clear().
 *
 * If the mode is not changed, that is, if the mode is already
 * set to the given mode, nothing will be changed. */
void ComDataCheck_set_mode(ComDataCheck* cdc, char input)
{
	if(!cdc || cdc->input == input)return;

	cdc->input = input;
	if(cdc->input)
		cdc->data_len = 0;
	else
		cdc->data_len = BinaryBuffer_get_length(cdc->buff);
}

void ComDataCheck_set_extended_data(ComDataCheck* cdc, void* extended_data, 
	alib_free_value free_extended_data)
{
	if(!cdc)return;

	if(cdc->extended_data && cdc->free_extended_data)
		cdc->free_extended_data(cdc->extended_data);

	cdc->extended_data = extended_data;
	cdc->free_extended_data = free_extended_data;
}
	/***********/
/******************************/

/*******Constructors*******/
/* Constructs a new ComDataCheck object. */
ComDataCheck* newComDataCheck(char input)
{
	ComDataCheck* cdc = malloc(sizeof(ComDataCheck));
	if(!cdc)return(NULL);

	/* Initialize non-dynamic members. */
	cdc->input = input;
	cdc->data_len = 0;
	cdc->extended_data = NULL;
	cdc->free_extended_data = NULL;

	/* Initialize dynamic members. */
	cdc->buff = newBinaryBuffer();
	cdc->unread_buff = newBinaryBuffer();

	if(!cdc->buff || !cdc->unread_buff)
		delComDataCheck(&cdc);

	return(cdc);
}

/* Frees the memory allocated by the ComDataCheck object. */
void freeComDataCheck(ComDataCheck* cdc)
{
	if(!cdc)return;

	delBinaryBuffer(&cdc->buff);
	delBinaryBuffer(&cdc->unread_buff);
	if(cdc->extended_data && cdc->free_extended_data)
		cdc->free_extended_data(cdc->extended_data);

	free(cdc);
}
/* Frees the object then sets the object's pointer to NULL. */
void delComDataCheck(ComDataCheck** cdc)
{
	if(cdc)
	{
		freeComDataCheck(*cdc);
		*cdc = NULL;
	}
}
/**************************/
