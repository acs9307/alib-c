#ifndef COM_DATA_CHECK_IS_DEFINED
#define COM_DATA_CHECK_IS_DEFINED

#include <stdlib.h>
#include <inttypes.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <alib-c/BinaryBuffer.h>
#include <alib-c/String.h>
#include <alib-c/server_defines.h>
//#include "BinaryBuffer.h"
//#include "String.h"
//#include "server_defines.h"

/* Used during data transmission so that the receiver knows if all
 * data has or hasn't been received yet.
 *
 * Output data structure is as follows.
 *
 * Data Length (in bytes) first four bytes (MUST BE IN NETWORK BYTE ORDER),
 * Remaining data is appended to the end.
 */
typedef struct ComDataCheck ComDataCheck;

typedef enum ComDataCheck_RVal
{
	CDC_LEN_ERR = ALIB_OBJ_CORRUPTION,
	CDC_COMPLETE = 0,
	CDC_WAITING = 1,
	/* Data is still waiting to be processed. */
	CDC_DATA_REMAINING = 2,
}ComDataCheck_RVal;

/*******Public Functions*******/
/* Checks the state of the ComDataCheck object and returns
 * a related ComDataCheck_RVal. */
int ComDataCheck_check(ComDataCheck* cdc);

/* Processes the unprocessed data in the ComDataCheck object by calling
 * 'ComDataCheck_append()' to append the unprocessed data onto the end of the object's buffer
.
 *
 * Returns the same value as 'ComDataCheck_append()'. */
int ComDataCheck_process_buffered_data(ComDataCheck* cdc);

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
int ComDataCheck_append(ComDataCheck* cdc, unsigned char* data, size_t data_len);

/* Clears any data stored in the ComDataCheck object. */
void ComDataCheck_clear(ComDataCheck* cdc);

/* Sends the data in the given ComDataCheck to the given 'sock'.
 *
 * Parameters:
 * 		cdc: The object containing the data to send.
 * 		sock: The socket of the node to send data to.
 * 		flags: The flags to use with 'send()'.
 */
alib_error ComDataCheck_send(ComDataCheck* cdc, int sock, int flags);
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
alib_error ComDataCheck_recv(ComDataCheck* cdc, int sock, int flags);

	/* Getters */
/* Returns the buffer of the ComDataCheck object.
 *
 * Assumes 'cdc' is not null. */
BinaryBuffer* ComDataCheck_get_buffer (ComDataCheck* cdc);
/* Returns the expected length of the data portion of the object.
 *
 * Assumes 'cdc' is not null. */
int ComDataCheck_get_expected_len(ComDataCheck* cdc);

void* ComDataCheck_get_extended_data(ComDataCheck* cdc);
	/***********/

	/* Setters */
/* Sets the mode of the ComDataCheck.  If the mode is changed,
 * the 'data_len' member will be updated accordingly, however
 * data will not be cleared.  To clear data, manually call
 * ComDataCheck_clear().
 *
 * If the mode is not changed, that is, if the mode is already
 * set to the given mode, nothing will be changed. */
void ComDataCheck_set_mode(ComDataCheck* cdc, char input);

void ComDataCheck_set_extended_data(ComDataCheck* cdc, void* extended_data,
	alib_free_value free_extended_data);
	/***********/
/******************************/

/*******Constructors*******/
/* Constructs a new ComDataCheck object. */
ComDataCheck* newComDataCheck(char input);

/* Frees the memory allocated by the ComDataCheck object. */
void freeComDataCheck(ComDataCheck* cdc);
/* Frees the object then sets the object's pointer to NULL. */
void delComDataCheck(ComDataCheck** cdc);
/**************************/

#endif
