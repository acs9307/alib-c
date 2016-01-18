#ifndef MEM_POOL_IS_DEFINED
#define MEM_POOL_IS_DEFINED

#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>

#include "alib_types.h"

#ifndef MEMPOOL_DEFAULT_MAX_CAPACITY
#define MEMPOOL_DEFAULT_MAX_CAPACITY 256
#endif

/*******FUNCTION POINTERS*******/
/* Called whenever the data portion of a MemPoolBlock
 * needs to be allocated.
 *
 * Parameters:
 * 		data: Should be set to the new data block.  If for
 * 			some reason an error occurs, data should be
 * 			set to NULL.
 * 		size: The size of the new data block in bytes. */
typedef void (*mem_pool_block_data_alloc_cb)(void**, size_t*);
/*******************************/

/*******OBJECT DECLARATIONS*******/
typedef struct MemPoolBlock MemPoolBlock;
/* Object that inherits from ArrayList.  Designed to allocate memory
 * in blocks for multiple uses per allocation.
 *
 * All functions from ArrayList can be used. */
typedef struct MemPool MemPool;
/*********************************/

/*******MEM_POOL_ITEM FUNCTIONS*******/
/* GETTERS */
/* Returns the data member of the block object.
 *
 * Assumes 'block' is not null. */
void* MemPoolBlock_get_data(MemPoolBlock* block);
/* Returns the size in bytes of the data member of
 * the block object.
 *
 * Assumes 'block' is not null. */
size_t MemPoolBlock_get_data_size(MemPoolBlock* block);

/* Returns whether or not the block is reserved.
 *
 * Assumes 'block' is not null.
 *
 * Returns:
 * 		1: The block is reserved.
 * 		0: The block is not reserved.
 */
char MemPoolBlock_is_reservered(MemPoolBlock* block);
	/***********/

	/* Lifecycle */
/* Creates a new MemPoolBlock. */
MemPoolBlock* newMemPoolBlock(MemPool* pool);

/* Frees a MemPoolBlock object and its internal memory.
 * This function does not worry about reservation.  Therefore
 * if anything has the block reserved, behavior is undefined.
 *
 * Parameters:
 * 		void_block: A void pointer to the MemPoolBlock object.
 * 			It is a void pointer to make the ArrayList's free_item
 * 			function pointer happy. */
void freeMemPoolBlock(void* void_block);
	/*************/
/*************************************/

/*******MEM_POOL FUNCTIONS*******/
/* Reserves a MemPoolBlock from the MemPool.
 * If there are no available blocks, the function
 * will try to resize.
 *
 * Parameters:
 * 		pool: The MemPool to search for an available block.
 *
 * Returns:
 * 		The pointer to the reserved memory block.  If the
 * 		pointer is null, then no blocks could be reserved. */
MemPoolBlock* MemPool_reserve_block(MemPool* pool);
/* Unreserves a MemPoolBlock and sets the given pointer
 * to NULL.
 *
 * Parameters:
 * 		block: A double pointer to the block to unreserve.
 * 			The pointer will be set to NULL upon
 * 			completion.
 */
void MemPool_unreserve_block(MemPoolBlock** block);

	/* CONSTRUCTORS */
/* Extended version of 'newMemPool()'. */
MemPool* newMemPool_ex(mem_pool_block_data_alloc_cb alloc_cb,
		alib_free_value free_cb, size_t start_cap, size_t max_cap, char use_mutex);
/* Creates a new MemPool object. */
MemPool* newMemPool(mem_pool_block_data_alloc_cb alloc_cb, alib_free_value free_cb);

/* Destroys a MemPool object. */
void delMemPool(MemPool** pool);
	/****************/
/********************************/

#endif
