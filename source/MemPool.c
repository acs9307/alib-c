#include "MemPool_private.h"

/*******MEM_POOL_ITEM*******/
	/* GETTERS */
/* Returns the data member of the block object.
 *
 * Assumes 'block' is not null. */
void* MemPoolBlock_get_data(MemPoolBlock* block)
{
	return(block->data);
}
/* Returns the size in bytes of the data member of
 * the block object.
 *
 * Assumes 'block' is not null. */
size_t MemPoolBlock_get_data_size(MemPoolBlock* block)
{
	return(block->data_size);
}

/* Returns whether or not the block is reserved.
 *
 * Assumes 'block' is not null.
 *
 * Returns:
 * 		1: The block is reserved.
 * 		0: The block is not reserved.
 */
char MemPoolBlock_is_reservered(MemPoolBlock* block)
{
	return(block->reserving_pool != NULL);
}
	/***********/

	/* Lifecycle */
/* Creates a new MemPoolBlock. */
MemPoolBlock* newMemPoolBlock(MemPool* pool)
{
	MemPoolBlock* block;

	/* Check for bad argument. */
	if(!pool)return(NULL);

	/* Allocate the memory for the object. */
	block = malloc(sizeof(MemPoolBlock));
	if(!block)
		return(NULL);

	/* Allocate the user data. */
	pool->alloc_cb(&block->data, &block->data_size);
	if(!block->data)
	{
		free(block);
		return(NULL);
	}

	/* Initialize other members. */
	block->free_data = pool->free_user_data_cb;
	block->reserving_pool = NULL;

	return(block);
}

/* Frees a MemPoolBlock object and its internal memory.
 * This function does not worry about reservation.  Therefore
 * if anything has the block reserved, behavior is undefined.
 *
 * Parameters:
 * 		void_block: A void pointer to the MemPoolBlock object.
 * 			It is a void pointer to make the ArrayList's free_item
 * 			function pointer happy. */
void freeMemPoolBlock(void* void_block)
{
	MemPoolBlock* block = (MemPoolBlock*)void_block;

	/* Free internal data. */
	if(block->data && block->free_data)
		block->free_data(block->data);

	/* Free the actual block. */
	free(block);
}
	/*************/
/***************************/

/*******MEM_POOL FUNCTIONS*******/
	/* PRIVATE CALLBACK FUNCTIONS */
/* Simply calls free on the block of memory.
 *
 * Parameters:
 * 		block: The block of memory to free.
 *
 * Type: alib_free_value */
static void free_block_default(void* block)
{
	if(block != NULL)
		free(block);
}
	/******************************/

	/* PRIVATE FUNCTIONS */
/* Fills the newly allocated memory in the array.
 *
 * If there are any null-pointers anywhere besides
 * the end of the array, blocks of memory will be overwritten
 * and memory leaks will occur.
 *
 * Parameters:
 * 		pool: The MemPool object to fill.
 */
static alib_error fill_resized_memory(MemPool* pool)
{
	void** array_it;
	alib_error r_code = ALIB_OK;

	if(!pool)return(ALIB_BAD_ARG);

	/* Lock the mutex if we are using mutexing. */
	if(pool->array->use_mutex)
		pthread_mutex_lock(&pool->array->mutex);

	/* Iterate through the list and fill the empty slots. */
	array_it = pool->array->list + pool->array->count;
	for(; pool->array->count < pool->array->capacity; ++array_it)
	{
		*array_it = newMemPoolBlock(pool);

		if(*array_it)
			++pool->array->count;
		else
		{
			r_code = ALIB_MEM_ERR;
			goto f_return;
		}
	}

	/* Unlock the mutex if we are using mutexing. */
	if(pool->array->use_mutex)
		pthread_mutex_unlock(&pool->array->mutex);

f_return:
	return(r_code);
}
/* Searches for NULL pointers within the array and sets
 * their values to new MemPoolBlock objects.
 *
 * Should only be called if some blocks within the array
 * have been freed.  Normally should call 'fill_resized_memory'
 * as it will only fill the last section of memory rather than
 * search every single item.
 *
 * Parameters:
 * 		pool: The MemPool object to fill.
 */
//static alib_error fill_memory(MemPool* pool)
//{
//	void** array_it;
//
//	if(!pool)return(ALIB_BAD_ARG);
//
//	/* Iterate through the list and look for empty slots.
//	 * If an empty slot  */
//	array_it = pool->array->list;
//	for(; pool->array->count < pool->array->capacity; ++array_it)
//	{
//		if(*array_it == NULL)
//		{
//			*array_it = newMemPoolBlock(pool);
//
//			if(*array_it)
//				++pool->array->count;
//			else
//				return(ALIB_MEM_ERR);
//		}
//	}
//
//	return(ALIB_OK);
//}

/* Resizes the MemPool's array to two times the
 * current capacity.
 *
 * Parameters:
 * 		pool: The memory pool to expand.
 */
static alib_error expand(MemPool* pool)
{
	size_t new_cap;

	if(!pool)return(ALIB_BAD_ARG);

	/* Check to see if we have hit our max capacity. */
	if(pool->array->capacity == pool->array->max_cap)
		return(ALIB_INTERNAL_MAX_REACHED);

	new_cap = pool->array->capacity * 2;
	if(new_cap < pool->array->capacity)
		new_cap = ULONG_MAX;

	/* Ensure we are able to allocate the new memory. */
	if(ArrayList_resize_tsafe(pool->array, new_cap) ==
			pool->array->count)
		return(ALIB_MEM_ERR);

	return(fill_resized_memory(pool));
}
	/*********************/

	/* PUBLIC FUNCTIONS */
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
MemPoolBlock* MemPool_reserve_block(MemPool* pool)
{
	void** array_it;
	size_t it_count;
	MemPoolBlock* block;

	/* Check for null args. */
	if(!pool)return(NULL);

	/* If there are no available blocks, then we need
	 * to try and expand the pool. */
	if(pool->reserved_count == pool->array->capacity &&
			expand(pool))
	{
		return(NULL);
	}

	/* Iterate through the list until we find a block that has not been reserved. */
	for(array_it = pool->array->list, it_count = 0; it_count < pool->array->capacity;
			++array_it, ++it_count)
	{
		block = (MemPoolBlock*)*array_it;
		if(!block->reserving_pool)
		{
			block->reserving_pool = pool;
			++pool->reserved_count;
			return(block);
		}
	}

	return(NULL);
}
/* Unreserves a MemPoolBlock and sets the given pointer
 * to NULL.
 *
 * Parameters:
 * 		block: A double pointer to the block to unreserve.
 * 			The pointer will be set to NULL upon
 * 			completion.
 */
void MemPool_unreserve_block(MemPoolBlock** block)
{
	if(!block || !*block)return;

	--(*block)->reserving_pool->reserved_count;
	(*block)->reserving_pool = NULL;
	*block = NULL;
}
	/********************/

	/* CONSTRUCTORS */
/* Extended version of 'newMemPool()'. */
MemPool* newMemPool_ex(mem_pool_block_data_alloc_cb alloc_cb,
		alib_free_value free_cb, size_t start_cap, size_t max_cap, char use_mutex)
{
	MemPool* pool;

	/* We must have an allocation callback function. */
	if(!alloc_cb)return(NULL);

	/* Allocate the object. */
	pool = malloc(sizeof(MemPool));
	if(!pool)return(NULL);

	/* Construct base. */
	pool->array = newArrayList_ex(freeMemPoolBlock, start_cap,
			max_cap, use_mutex);
	if(!pool->array)
	{
		free(pool);
		return(NULL);
	}

	/* Initialize other members. */
	pool->alloc_cb = alloc_cb;
	pool->free_user_data_cb = free_cb;
	pool->reserved_count = 0;

	/* Fill the array with data. */
	if(fill_resized_memory(pool))
		delMemPool(&pool);

	return(pool);
}
/* Creates a new MemPool object. */
MemPool* newMemPool(mem_pool_block_data_alloc_cb alloc_cb, alib_free_value free_cb)
{
	return(newMemPool_ex(alloc_cb, (free_cb)?free_cb:free_block_default,
			1, MEMPOOL_DEFAULT_MAX_CAPACITY, 1));
}

/* Destroys a MemPool object. */
void delMemPool(MemPool** pool)
{
	if(!pool || !*pool)return;

	delArrayList(&(*pool)->array);

	free(*pool);
	*pool = NULL;
}
	/****************/
/********************************/
