#ifndef COM_DATA_CHECK_PRIVATE_IS_DEFINED
#define COM_DATA_CHECK_PRIVATE_IS_DEFINED

#include "ComDataCheck.h"

/* Used during data transmission so that the receiver knows if all
 * data has or hasn't been received yet.
 *
 * Output data structure is as follows.
 *
 * Data Length (in bytes) first four bytes (MUST BE IN NETWORK BYTE ORDER),
 * Remaining data is appended to the end.
 */
struct ComDataCheck
{
	/* The expected length of the data. */
	int32_t data_len;
	/* The buffer storing the data. */
	BinaryBuffer* buff;
	/* Buffer of unprocessed data. Simply used to store data that, after being 
	 * read, has remaining data after the end of the original data. */
	BinaryBuffer* unread_buff;
	
	/* If true, the package will be used to store
	 * and check data received.  Otherwise it will
	 * prepare data to write to a connection. */
	char input;

	void* extended_data;
	alib_free_value free_extended_data;
};

#endif
