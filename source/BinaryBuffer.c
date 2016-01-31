#include "BinaryBuffer_private.h"

/*******Private Functions*******/
/* Calculates what the next expansion size would be provided the given size. */
static void calculate_expansion(BinaryBuffer* buff, size_t* size)
{
	if(!*size)
		*size = 1;
	else
	{
		size_t new_size = (*size > buff->max_expand) ?
				*size + buff->max_expand : *size << 1;
		if(new_size < *size)
			*size = ULONG_MAX;
		else
			*size = new_size;
	}
}


/* Private resize resizes the allocated memory to be exactly
 * the number of bytes long received.  This differs from the public
 * resize function in that we do not calculate whether or not the
 * allocated space is below the object's minimum.
 *
 * If an error returns, memory will not have been modified. */
alib_error BinaryBuffer_hard_resize(BinaryBuffer* buff, size_t new_size)
{
	if(!buff)return(ALIB_BAD_ARG);

	if(buff->capacity == new_size)
		return(ALIB_OK);

	/* A size of zero was given, we simply need to free everything. */
	if(!new_size)
	{
		if(buff->buff)
			free(buff->buff);
		buff->buff = NULL;
		buff->capacity = 0;
		buff->len = 0;
	}
	/* Use realloc() to resize the memory. */
	else
	{
		buff->buff = realloc(buff->buff, new_size);
		if(!buff->buff)
			return(ALIB_MEM_ERR);

		buff->capacity = new_size;
		if(buff->len > new_size)
			buff->len = new_size;
	}

	return(ALIB_OK);
}
/*******************************/

/*******Public Functions*******/

/* Attempts to resize the buffer capacity by 2 times or by
 * the BinaryBuffer's maximum expansion value, whichever is smaller.
 */
alib_error BinaryBuffer_expand(BinaryBuffer* buff)
{
	size_t new_cap;

	/* Ensure we can expand. */
	if(!buff)return(ALIB_BAD_ARG);
	if(buff->capacity == ULONG_MAX)
		return(ALIB_INTERNAL_MAX_REACHED);

	/* Find a new buffer size for expansion. */
	calculate_expansion(buff, &new_cap);

	/* Resize. */
	return(BinaryBuffer_resize(buff, new_cap));
}
/* Works similarly to BinarBuffer_expand, except it will continue to expand
 * until the capacity is greater than or equal to the targeted capacity.
 *
 * This is more efficient if you must expand more than one time. */
alib_error BinaryBuffer_expand_to_target(BinaryBuffer* buff, size_t target)
{
	/* Ensure we can expand. */
	if(!buff)return(ALIB_BAD_ARG);

	/* Calculate what the expanded capacity will be. */
	size_t new_cap = buff->capacity;
	while(new_cap < target)
		calculate_expansion(buff, &new_cap);

	/* Resize the buffer. */
	return(BinaryBuffer_resize(buff, new_cap));
}

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
alib_error BinaryBuffer_resize(BinaryBuffer* buff, size_t new_size)
{
	if(!buff)return(ALIB_BAD_ARG);

	if(new_size < buff->min_cap)
		new_size = buff->min_cap;

	return(BinaryBuffer_hard_resize(buff, new_size));
}

/* Appends a block of data to the end of the buffer.
 * The internal buffer memory will be expanded/doubled until the given data can
 * fit into the buffer.
 *
 * Parameters:
 * 		buff: The BinaryBuffer to append to.
 * 		data: The data to append to the buffer.
 * 		data_len: The number of bytes to append to the buffer.
 */
alib_error BinaryBuffer_append(BinaryBuffer* buff, const void* data, size_t data_len)
{
	if(!buff || !data)return(ALIB_BAD_ARG);
	if(buff->len + data_len > ULONG_MAX)return(ALIB_INTERNAL_MAX_REACHED);

	size_t new_data_len = buff->len + data_len;

	if(buff->capacity < new_data_len)
	{
		alib_error err;
		if((err = BinaryBuffer_expand_to_target(buff, new_data_len)))
			return(err);
	}

	/* Copy the memory over. */
	memcpy((buff->buff + buff->len), data, data_len);
	buff->len = new_data_len;
	return(ALIB_OK);
}
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
alib_error BinaryBuffer_insert(BinaryBuffer* buff, size_t index, const void* data,
		size_t data_len)
{
	if(!buff || !data)return(ALIB_BAD_ARG);
	if(index > buff->len)return(ALIB_BAD_INDEX);
	if(buff->len + data_len > ULONG_MAX)return(ALIB_INTERNAL_MAX_REACHED);

	size_t new_len = buff->len + data_len;

	/* Expand to fit the required length. */
	if(buff->capacity < new_len)
	{
		alib_error err;
		if((err = BinaryBuffer_expand_to_target(buff, new_len)))
			return(err);
	}

	/* Copy the memory around. */
	memcpy_back(buff->buff + index + data_len, buff->buff + index,
			buff->len - index);
	memcpy(buff->buff + index, data, data_len);
	buff->len = new_len;

	return(ALIB_OK);
}

/* Clears all the memory in the internal buffer.  This does not
 * modify the values within memory, but modifies the BinaryBuffer's
 * length member.
 *
 * If the capacity exceeds the buffer's minimum capacity
 * then the internal buffer will be freed. */
void BinaryBuffer_clear(BinaryBuffer* buff)
{
	if(buff)
	{
		if(buff->capacity > buff->min_cap)
			BinaryBuffer_hard_resize(buff, 0);
		else
			buff->len = 0;
	}
}
/* Works similarly to BinaryBuffer_clear() but if the buffer's
 * capacity exceeds the minimum, no memory will be freed. */
void BinaryBuffer_clear_no_resize(BinaryBuffer* buff)
{
	if(buff)
		buff->len = 0;
}
/* Shrinks the BinaryBuffer's buffer to fit size of the data
 * currently stored. */
void BinaryBuffer_shrink_to_fit(BinaryBuffer* buff)
{
	BinaryBuffer_hard_resize(buff, buff->len);
}

/* Removes data from the BinaryBuffer from the 'begin' to the 'end' index.
 *
 * Parameters:
 * 		buff: The buffer to modify.
 * 		begin: The index to start removing from (inclusive).
 * 		end: The index to stop removing from (exclusive).
 */
alib_error BinaryBuffer_remove(BinaryBuffer* buff, size_t begin, size_t end)
{
	if(!buff)return(ALIB_BAD_ARG);

	if(begin > end)
	{
		size_t tmp = end;
		end = begin;
		begin = tmp;
	}

	if(end >= buff->len - 1)
		buff->len = begin;
	else
	{
		memcpy(buff->buff + begin, buff->buff + end, buff->len - end);
		buff->len -= end - begin;
	}

	return(ALIB_OK);
}

/* Copy memory from the BinaryBuffer to a user allocated buffer.
 *
 * Parameters:
 * 		buff: The BinaryBuffer to copy data from.
 * 		to: The user defined buffer to copy data into.
 * 		count: The size of 'to' in bytes. If larger than
 * 			the length of the actual 'to' buffer, then behavior
 * 			is undefined. */
alib_error BinaryBuffer_copy(BinaryBuffer* buff, void* to, size_t count)
{
	if(!buff || !to)return(ALIB_BAD_ARG);

	memcpy(to, buff->buff, (count < buff->len)?count:buff->len);
	return(ALIB_OK);
}
/* Copies memory from the BinaryBuffer to a user defined buffer. The data
 * that is copied is then removed from the BinaryBuffer.
 *
 * Parameters:
 * 		buff: The BinaryBuffer to copy data from.
 * 		to: The user defined buffer to copy data into.
 * 		count: The size of 'to' in bytes. If larger than
 * 			the length of the actual 'to' buffer, then behavior
 * 			is undefined. */
alib_error BinaryBuffer_copy_and_drain(BinaryBuffer* buff, void* to, size_t count)
{
	if(!buff || !to)return(ALIB_BAD_ARG);

	if(count > buff->len)
		count = buff->len;
	memcpy(to, buff->buff, count);
	return(BinaryBuffer_remove(buff, 0, count));
}
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
		void* to, size_t count)
{
	if(!buff)return(ALIB_BAD_ARG);

	/* Ensure the 'begin' and 'end' are valid. */
	if(begin > end)
	{
		size_t index = begin;
		begin = end;
		end = index;
	}
	if(end >= buff->len)
		return(ALIB_BAD_INDEX);

	/* Find the largest number of bytes that can be copied. */
	end = end - begin;
	if(count > end)
		count = end;

	memcpy(to, buff->buff + begin, count);
	return(ALIB_OK);
}
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
		void* to, size_t count)
{
	/* Copy. */
	int err = BinaryBuffer_copy_block(buff, begin, end, to, count);
	if(err)return(err);

	/* Find how many bytes we removed. */
	{
		size_t new_count = end - begin;
		if(count > new_count)
			count = new_count;
	}

	/* Remove. */
	err = BinaryBuffer_remove(buff, begin, begin + count);
	return(err);
}

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
void* BinaryBuffer_extract_buffer(BinaryBuffer* buff)
{
	if(!buff)return(NULL);

	void* rval = buff->buff;
	buff->buff = NULL;
	buff->capacity = 0;
	buff->len = 0;

	return(rval);
}

/* Writes the BinaryBuffer to a file pointer.  This is simply a convenience
 * function.
 *
 * Assumes 'buff' and 'file' are not null. */
size_t BinaryBuffer_write_to_file(BinaryBuffer* buff, FILE* file)
{
	return(fwrite(buff->buff, 1, buff->len, file));
}

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
		const void* new_data, size_t new_data_len)
{
	if(!buff || index > buff->len)return(ALIB_BAD_ARG);

	/* Calculate out the index ptr. */
	unsigned char* index_ptr = buff->buff + index;

	/* Expand memory as needed. */
	if(buff->capacity < buff->len + new_data_len - old_len)
	{
		alib_error err = BinaryBuffer_expand_to_target(buff,
				buff->len + new_data_len - old_len);
		if(err)return(err);
	}

	/* Resize and copy existing data into a later position in the buffer. */
	if(old_len != new_data_len)
		memcpy_safe(index_ptr + new_data_len,
				index_ptr + old_len, buff->len - index - old_len);

	/* Copy the new data into the buffer. */
	memcpy(index_ptr, new_data, new_data_len);

	/* Modify length. */
	buff->len += new_data_len - old_len;

	return(ALIB_OK);
}

	/* Getters */
/* Returns a pointer to the raw internal buffer.
 * READONLY - MODIFY AT YOUR OWN RISK.
 *
 * Assumes 'buff' is not null. */
const void* BinaryBuffer_get_raw_buff(BinaryBuffer* buff)
{
	return(buff->buff);
}
/* Returns the length of the internal buffer in bytes.
 *
 * Assumes 'buff' is not null. */
size_t BinaryBuffer_get_length(BinaryBuffer* buff)
{
	return(buff->len);
}
/* Returns the capacity of the internal buffer in bytes.
 *
 * Assumes 'buff' is not null. */
size_t BinaryBuffer_get_capacity(BinaryBuffer* buff)
{
	return(buff->capacity);
}
/* Returns the minimum capacity of the internal buffer in bytes.
 *
 * Assumes 'buff' is not null. */
size_t BinaryBuffer_get_min_capacity(BinaryBuffer* buff)
{
	return(buff->min_cap);
}
/* Returns the maximum number of bytes the internal buffer may
 * expand per iteration.
 *
 * Assumes 'buff' is not null. */
size_t BinaryBuffer_get_max_expand_size(BinaryBuffer* buff)
{
	return(buff->max_expand);
}
	/***********/

	/* Setters */
/* Sets the minimum capacity of the internal buffer in bytes.
 *
 * Assumes 'buff' is not null. */
void BinaryBuffer_set_min_capacity(BinaryBuffer* buff, size_t min_cap)
{
	buff->min_cap = min_cap;
}
/* Sets the maximum number of bytes the internal buffer may expand by.
 * If 'max_expand' is set to zero, the buffer's value will be set to
 * ULONG_MAX.
 *
 * Assumes 'buff' is not null. */
void BinaryBuffer_set_max_expand_size(BinaryBuffer* buff, size_t max_expand)
{
	if(max_expand)
		buff->max_expand = max_expand;
	else
		buff->max_expand = ULONG_MAX;
}
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
 * 			be set to ULONG_MAX.
 */
BinaryBuffer* newBinaryBuffer_ex(unsigned char* data, size_t data_len,
		size_t start_cap, size_t min_cap, size_t max_expand)
{
	BinaryBuffer* buff = malloc(sizeof(BinaryBuffer));
	if(!buff)return(NULL);

	/* Check argument values. */
	if(data_len > start_cap)
		start_cap = data_len;
	if(min_cap > start_cap)
		start_cap = min_cap;

	/* Set non-dynamic members. */
	buff->capacity = start_cap;
	buff->len = data_len;
	buff->min_cap = min_cap;
	if(max_expand)
		buff->max_expand = max_expand;
	else
		buff->max_expand = ULONG_MAX;

	/* Allocate internal memory. */
	if(start_cap)
	{
		if(!(buff->buff = malloc(start_cap)))
			delBinaryBuffer(&buff);
	}
	else
		buff->buff = NULL;

	/* Fill buffer. */
	if(data && data_len)
		memcpy(buff->buff, data, data_len);

	return(buff);
}
/* Instantiates a new BinaryBuffer with a default minimum memory capacity. */
BinaryBuffer* newBinaryBuffer()
{
	return(newBinaryBuffer_ex(NULL, 0, 0, BINARY_BUFFER_DEFAULT_MIN_CAP,
			BINARY_BUFFER_DEFUALT_MAX_EXPAND));
}
/* Deletes a BinaryBuffer meaning all memory allocated by the object is
 * freed and the pointer is set to NULL. */
void delBinaryBuffer(BinaryBuffer** buff)
{
	if(!buff || !*buff)return;

	if((*buff)->buff)
		free((*buff)->buff);

	free(*buff);
	*buff = NULL;
}
/***********************/
