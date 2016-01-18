#ifndef BINARY_BUFFER_IS_INCLUDED
#define BINARY_BUFFER_IS_INCLUDED

#include <string.h>
#include <stdlib.h>
#include <inttypes.h>

#include "alib_error.h"
#include "alib_string.h"

#define BINARY_BUFFER_DEFAULT_MIN_CAP 64
#define BINARY_BUFFER_DEFUALT_MAX_EXPAND 1024 * 1024 //1 MB.

/* Buffer object used to dynamically store data safely and efficiently.
 * Allocated memory exponentially grows.
 *
 * NOTES:
 * 		No function which modifies memory, either by reallocating or setting, will make
 * 			any modification if an error occurs.
 * 		This is not built with thread safety, to safely operate on the object, you must
 * 			build a wrapper object. */
typedef struct BinaryBuffer BinaryBuffer;

/*******Public Functions*******/

/* Attempts to resize the buffer capacity by 2 times or by
 * the BinaryBuffer's maximum expansion value, whichever is smaller.
 */
alib_error BinaryBuffer_expand(BinaryBuffer* buff);
/* Works similarly to BinarBuffer_expand, except it will continue to expand
 * until the capacity is greater than or equal to the targeted capacity.
 *
 * This is more efficient if you must expand more than one time. */
alib_error BinaryBuffer_expand_to_target(BinaryBuffer* buff, size_t target);

/* Simply resizes the */
/* Resizes the BinaryBuffer's buffer to equal the 'new_size' unless
 * the new size is smaller than the minimum capacity.
 *
 * If resizing to a smaller capacity, any old data that exceeds the new
 * capacity will be truncated.
 *
 * Parameters:
 * 		'buff': The BinaryBuffer to modify.
 * 		'new_size': The new size to resize the internal buffer to.
 */
alib_error BinaryBuffer_resize(BinaryBuffer* buff, size_t new_size);

/* Appends a block of data to the end of the buffer.
 * The internal buffer memory will be expanded/doubled until the given data can
 * fit into the buffer.
 *
 * Parameters:
 * 		buff: The BinaryBuffer to append to.
 * 		data: The data to append to the buffer.
 * 		data_len: The number of bytes to append to the buffer.
 */
alib_error BinaryBuffer_append(BinaryBuffer* buff, const void* data, size_t data_len);
/* Inserts a block of data into the BinaryBuffer at the specified index.
 *
 * Parameters:
 * 		buff: The BinaryBuffer to modify.
 * 		index: The index to insert the data into. (All bytes at and after this index
 * 			will be placed after the new data.  Therefore if insert was used on a
 * 			string such as 'hello world' at index of 5, then the string would be
 * 			'hello{data} world' upon return.)
 * 		data: The block of data to insert.
 * 		data_len: The length of the data in bytes. */
alib_error BinaryBuffer_insert(BinaryBuffer* buff, size_t index, const void* data, size_t data_len);

/* Clears all the memory in the internal buffer.  This does not
 * modify the values within memory, but modifies the BinaryBuffer's
 * length member.
 *
 * If the capacity exceeds the buffer's minimum capacity
 * then the internal buffer will be freed. */
void BinaryBuffer_clear(BinaryBuffer* buff);
/* Works similarly to BinaryBuffer_clear() but if the buffer's
 * capacity exceeds the minimum, no memory will be freed. */
void BinaryBuffer_clear_no_resize(BinaryBuffer* buff);
/* Shrinks the BinaryBuffer's buffer to fit size of the data
 * currently stored. */
void BinaryBuffer_shrink_to_fit(BinaryBuffer* buff);

/* Removes data from the BinaryBuffer from the 'begin' to the 'end' index.
 *
 * Parameters:
 * 		buff: The buffer to modify.
 * 		begin: The index to start removing from (inclusive).
 * 		end: The index to stop removing from (exclusive).
 */
alib_error BinaryBuffer_remove(BinaryBuffer* buff, size_t begin, size_t end);

/* Copy memory from the BinaryBuffer to a user allocated buffer.
 *
 * Parameters:
 * 		buff: The BinaryBuffer to copy data from.
 * 		to: The user defined buffer to copy data into.
 * 		count: The size of 'to' in bytes. If larger than
 * 			the length of the actual 'to' buffer, then behavior
 * 			is undefined. */
alib_error BinaryBuffer_copy(BinaryBuffer* buff, void* to, size_t count);
/* Copies memory from the BinaryBuffer to a user defined buffer. The data
 * that is copied is then removed from the BinaryBuffer.
 *
 * Parameters:
 * 		buff: The BinaryBuffer to copy data from.
 * 		to: The user defined buffer to copy data into.
 * 		count: The size of 'to' in bytes. If larger than
 * 			the length of the actual 'to' buffer, then behavior
 * 			is undefined. */
alib_error BinaryBuffer_copy_and_drain(BinaryBuffer* buff, void* to, size_t count);
/* Similar to BinaryBuffer_copy() but the memory that is copies is within the two
 * indices rather than from the beginning of the BinaryBuffer.
 *
 * Parameters:
 * 		buff: The BinaryBuffer to copy from.
 * 		begin: The index to begin copying from (inclusive).
 * 		end: The index to stop copying from (exclusive).
 * 		to: The user defined buffer to copy data into.
 * 		count: The size of 'to' in bytes.  If larger than the
 * 			actual size of 'to', behavior is undefined. */
alib_error BinaryBuffer_copy_block(BinaryBuffer* buff, size_t begin, size_t end,
		void* to, size_t count);
/* Same as BinaryBuffer_copy_block() but the the memory that is copied will be removed from
 * the buffer.
 *
 * Parameters:
 * 		buff: The BinaryBuffer to copy from.
 * 		begin: The index to begin copying from (inclusive).
 * 		end: The index to stop copying from (exclusive).
 * 		to: The user defined buffer to copy data into.
 * 		count: The size of 'to' in bytes.  If larger than the
 * 			actual size of 'to', behavior is undefined.
 */
alib_error BinaryBuffer_copy_block_and_drain(BinaryBuffer* buff, size_t begin, size_t end,
		void* to, size_t count);

/* Returns the internal buffer of the BinaryBuffer and sets the
 * BinaryBuffers buffer to NULL.  This means that the returned memory
 * is no longer handled by the BinaryBuffer and MUST BE FREED by the caller.
 *
 * If you need to know the length of the buffer, you must check by calling
 * 'BinaryBuffer_get_length()' before calling this as the buffer's members
 * will be reset to 0.
 *
 * Parameters:
 * 		buff: The BinaryBuffer to extract data from.
 *
 * Returns a pointer to the internal buffer.  This buffer is no longer part
 * of the object and MUST BE FREED by the caller when no longer needed. */
void* BinaryBuffer_extract_buffer(BinaryBuffer* buff);

/* Writes the BinaryBuffer to a file pointer.  This is simply a convenience
 * function.
 *
 * Assumes 'buff' and 'file' are not null. */
size_t BinaryBuffer_write_to_file(BinaryBuffer* buff, FILE* file);

/* Replaces a section of data in the buffer with new data.
 *
 * If 'new_data' belongs to the buffer, behavior is undefined.
 *
 * Parameters:
 * 		buff: The BinaryBuffer to modify.
 * 		index: The index to replace.
 * 		old_len: The number of bytes to replace.
 * 		new_data: The data that should replace the existing data.
 * 		new_data_len: The length of the new data array. */
alib_error BinaryBuffer_replace(BinaryBuffer* buff, size_t index, size_t old_len,
		const void* new_data, size_t new_data_len);

	/* Getters */
/* Returns a pointer to the raw internal buffer.
 * READONLY - MODIFY AT YOUR OWN RISK.
 *
 * Assumes 'buff' is not null. */
const void* BinaryBuffer_get_raw_buff(BinaryBuffer* buff);
/* Returns the length of the internal buffer in bytes.
 *
 * Assumes 'buff' is not null. */
size_t BinaryBuffer_get_length(BinaryBuffer* buff);
/* Returns the capacity of the internal buffer in bytes.
 *
 * Assumes 'buff' is not null. */
size_t BinaryBuffer_get_capacity(BinaryBuffer* buff);
/* Returns the minimum capacity of the internal buffer in bytes.
 *
 * Assumes 'buff' is not null. */
size_t BinaryBuffer_get_min_capacity(BinaryBuffer* buff);
/* Returns the maximum number of bytes the internal buffer may
 * expand per iteration.
 *
 * Assumes 'buff' is not null. */
size_t BinaryBuffer_get_max_expand_size(BinaryBuffer* buff);
	/***********/

	/* Setters */
/* Sets the minimum capacity of the internal buffer in bytes.
 *
 * Assumes 'buff' is not null. */
void BinaryBuffer_set_min_capacity(BinaryBuffer* buff, size_t min_cap);
/* Sets the maximum number of bytes the internal buffer may expand by.
 * If 'max_expand' is set to zero, the buffer's value will be set to
 * SIZE_MAX.
 *
 * Assumes 'buff' is not null. */
void BinaryBuffer_set_max_expand_size(BinaryBuffer* buff, size_t max_expand);
	/***********/
/******************************/


/*******Lifecycle*******/
/* Instantiates a new BinaryBuffer with detailed data.  Usually
 * newBinaryBuffer() is sufficient.
 *
 * Parameters:
 * 		data: The data to place into the buffer upon instantiation.
 * 		data_len: The length of the data in bytes.
 * 		start_cap: The capacity to start the buffer with.
 * 		min_cap: The minimum capacity that should be allocated
 * 			for the buffer.
 * 		max_expand: (OPTIONA) The number of bytes that the buffer may expand
 * 			by per iteration.  If set to zero, then the maximum expansion will
 * 			be set to SIZE_MAX.
 */
BinaryBuffer* newBinaryBuffer_ex(unsigned char* data, size_t data_len,
		size_t start_cap, size_t min_cap, size_t max_expand);
/* Instantiates a new BinaryBuffer with a default minimum memory capacity. */
BinaryBuffer* newBinaryBuffer();

/* Deletes a BinaryBuffer meaning all memory allocated by the object is
 * freed and the pointer is set to NULL. */
void delBinaryBuffer(BinaryBuffer** buff);
/***********************/


#endif
