#ifndef MEM_POOL_PRIVATE_IS_DEFINED
#define MEM_POOL_PRIVATE_IS_DEFINED

#include "MemPool.h"
#include "ArrayList_private.h"
#include "alib_types.h"
#include "alib_error.h"

/*******MEM_POOL_ITEM*******/
struct MemPoolBlock
{
	/* Pointer to the raw data. */
	void* data;
	/* The size of the data in bytes. */
	size_t data_size;

	/* The callback to free the user defined data. */
	alib_free_value free_data;

	/* Pointer to the parent pool. Used for
	 * unreserving the block. It should be null until
	 * reserved. */
	MemPool* reserving_pool;
};
/***************************/

/*******MEM_POOL*******/
struct MemPool
{
	/* The array that the memory will be stored in.
	 * Will store MemPoolItems. */
	ArrayList* array;

	/* Number of blocks currently reserved, this differs from the ArrayList's
	 * 'count' member in that the ArrayList's value reflects how many blocks
	 * of memory we have stored in the list. */
	size_t reserved_count;

	/* Called whenever new blocks of memory must
	 * be allocated. */
	mem_pool_block_data_alloc_cb alloc_cb;
	/* Called whenever the user's data should be freed. */
	alib_free_value free_user_data_cb;
};
/**********************/

#endif
