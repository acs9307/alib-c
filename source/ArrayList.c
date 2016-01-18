#include "ArrayList_private.h"

/*******PROTECTED FUNCTIONS*******/
/* Protected function used to remove items from the list pointer then
 * sets the pointer to NULL. This is useful when iterating through the list
 * removing items as you go because it does not search for the item, it assumes
 * a valid pointer is passed.  Will not call the delete callback on the item.
 *
 * If a non-list pointer, or a list pointer not connected to the passed list
 * is given, behavior is undefined.
 *
 * Parameters:
 * 		list: The list to remove the item from.
 * 		item: The item to remove from the list. */
void ArrayList_remove_by_ptr_no_free(ArrayList* list, void** item)
{
	if(!list || !item)return;

	*item = NULL;
	--list->count;
}
/* Protected function used to remove items from the list pointer then
 * sets the pointer to NULL. This is useful when iterating through the list
 * removing items as you go because it does not search for the item, it assumes
 * a valid pointer is passed.  Delete callback will be called on the item after
 * it is removed from the list.
 *
 * If a non-list pointer, or a list pointer not connected to the passed list
 * is given, behavior is undefined.
 *
 * Parameters:
 * 		list: The list to remove the item from.
 * 		item: The item to remove from the list. */
void ArrayList_remove_by_ptr(ArrayList* list, void** item)
{
	void* item_holder;

	if(!list || !item)return;

	item_holder = *item;
	*item = NULL;
	--list->count;

	if(list->free_item)
		list->free_item(item_holder);
}
/*********************************/

/*******PUBLIC FUNCTIONS*******/
/* Adds an item to the array list. If the list is not large enough to store the
 * item, then allocated memory is doubled until it reaches the maximum size.
 *
 * Parameters:
 * 		list: The ArrayList to add the item to.
 * 		item: The item to add to the list.
 *
 * Returns:
 * 		1: Success.
 * 		0: Error.  Should only be caused when passing a null pointer or when the
 * 			list cannot expand anymore because of the max length value. */
char ArrayList_add(ArrayList* list, void* item)
{
	int** item_it;
	size_t item_count;

	/* Check for error. */
	if(!list || !item)return(0);

	/* Search for an empty slot in the list. */
	if(list->count < list->capacity)
	{
		item_it = (int**)list->list;
		item_count = 0;
		for(; item_count < list->capacity; ++item_count, ++item_it)
		{
			/* If we found a null spot, then we set the value and return true. */
			if(!*item_it)
			{
				*item_it = item;
				++list->count;
				return 1;
			}
		}
	}

	/* The list is full and cannot be expanded anymore, return false. */
	if(list->capacity == list->max_cap)
		return(0);
	/* No list exists, we just set the capacity to 1. */
	else if(list->capacity == 0)
		list->capacity = 1;
	/* Check to see if the highest order bit is set, if so, then
	 * we set the value to the maximum size. */
	else if(list->capacity & INT64_MIN)
		list->capacity = list->max_cap;
	/* Increment the capacity by two times. */
	else
		list->capacity = list->capacity << 1;

	/* Reallocate the memory. */
	list->list = realloc(list->list, list->capacity * sizeof(int*));
	item_it = (int**)list->list + list->count;

	/* Iterate backwards through all the new items in the list and set
	 * them to null. */
	for(item_count = list->capacity; item_count > list->count;
			--item_count, ++item_it)
		*item_it = NULL;

	/* Set an empty slot to the given value.  This slot should be the next slot
	 * in the list. */
	item_it = ((int**)list->list) + list->count;
	*item_it = item;
	++list->count;

	return(1);
}
/* Removes an item from the list by searching for an item pointing to the same
 * memory address. If the item is found it will be removed from the list, but
 * the delete callback will not be called on it.
 *
 * Parameters:
 * 		list: The list to remove the item from.
 * 		item: The item that should be removed. */
void ArrayList_remove_no_free(ArrayList* list, void* item)
{
	int** item_it;
	size_t item_count;

	if(!list || !item)return;

	item_it = (int**)list->list;
	for(item_count = 0; item_count < list->capacity; ++item_count, ++item_it)
	{
		if(*item_it == item)
		{
			ArrayList_remove_by_ptr_no_free(list, (void**)item_it);
			return;
		}
	}
}
/* Removes an item from the list by searching for an item pointing to the same
 * memory address. If the item is found, it will be removed from the list
 * and the delete callback will be called on it.
 *
 * Parameters:
 * 		list: The list to remove the item from.
 * 		item: The item that should be removed. */
void ArrayList_remove(ArrayList* list, void* item)
{
	int** item_it;
	size_t item_count;

	if(!list || !item)return;

	item_it = (int**)list->list;
	for(item_count = 0; item_count < list->capacity; ++item_count, ++item_it)
	{
		if(*item_it == item)
		{
			ArrayList_remove_by_ptr(list, (void**)item_it);
			return;
		}
	}
}
/* Gets the first item found from the list.  Searching starts at the
 * beginning of the array and ends at the end.
 *
 * Parameters:
 * 		list: The list to search in.
 *
 * Returns:
 * 		void*: Pointer to the first item found.
 * 		NULL: No item found or an error occurred.
 */
void* ArrayList_get_first_item(ArrayList* list)
{
	void** list_it;
	size_t it_count;

	/* Check for error. */
	if(!list)return NULL;

	/* Iterate through all the items in the list and return the item that we find. */
	for(list_it = list->list, it_count = 0; it_count < list->capacity; ++it_count, ++list_it)
	{
		/* If it is not null, then it is a valid item. */
		if(*list_it)
			return(*list_it);
	}
	return(NULL);
}
/* Finds the item pointer by the given value.
 *
 * Parameters:
 * 		list: The array list that should be searched.
 * 		val: A pointer to the value that should be used for comparison.
 * 		compare_cb: The callback used to compare the objects.
 *
 * Returns:
 * 		NULL: Error occurred or no matching value found.
 * 		void*: The pointer to the value that matches the given 'val' according to the 'compare_cb'.
 */
const void* ArrayList_find_item_by_value(ArrayList* list, void* val, alib_compare_values compare_cb)
{
	void** val_it;
	size_t val_count;

	if(!list || !compare_cb)return(NULL);

	val_count = list->count;
	for(val_it = list->list; val_count > 0;++val_it)
	{
		if(!*val_it)continue;

		if(compare_cb(val, *val_it) == 0)
			return(*val_it);
		else
			--val_count;
	}

	return(NULL);
}

/* Removes all the items from the list without actually deleting the list.
 *
 * Parameters:
 * 		list: The ArrayList to clear. */
void ArrayList_clear(ArrayList* list)
{
	void** item_it;

	/* Check for errors. */
	if(!list)return;

	/* Increment and remove each item from the list. */
	for(item_it = list->list; list->count > 0; ++item_it)
	{
		if(*item_it)
		{
			if(list->free_item)
				list->free_item(*item_it);
			*item_it = NULL;
			--list->count;
		}
	}
}

/* Resizes the list so that the capacity matches 'newcap'.
 * If the given array list's count is larger than the new
 * capacity, then the pointer list will be freed, but the
 * actual items will not be freed.  This will, however,
 * copy as many items in the current list to the new list
 * before removing items.
 *
 * Items are removed so that items nearest the beginning are
 * most likely to be kept.
 *
 * Returns:
 * 		New capacity of the list.  If the given 'list' is
 * 			NULL, then 0 will be returned.
 */
size_t ArrayList_resize(ArrayList* list, size_t newcap)
{
	void** temp_list = NULL;
	size_t i;
	void** list_it, **temp_it;

	/* Check for error. */
	if(!list)return(0);

	if(newcap > 0)
	{
		if(newcap < list->capacity)
		{
			/* Allocate memory for the new list.
			 * Must make a new list rather than simply reallocate
			 * because list data may not be contiguous. */
			temp_list = calloc(newcap, sizeof(void*));

			/* Copy each pointer over to the new list. */
			for(i = 0, list_it = list->list, temp_it = temp_list;
					i < list->count && i < newcap; ++list_it)
			{
				if(*list_it)
				{
					*temp_it = *list_it;
					++temp_it;
					++i;
				}
			}

			/* Truncate data that can't fit into the new list. */
			for(; list->count > newcap; ++list_it)
			{
				if(*list_it)
					ArrayList_remove_by_ptr(list, list_it);
			}

			/* Free the list and set the list to the new list. */
			free(list->list);
			list->list = temp_list;
		}
		else
		{
			list->list = realloc(list->list, sizeof(void*) * newcap);

			/* Nullify all the new pointers. */
			for(list_it = list->list + list->capacity; list->capacity < newcap;
					++list->capacity, ++list_it)
				*list_it = NULL;
		}
	}
	/* We are just supposed to free everything. */
	else
	{
		free(list->list);
		list->list = NULL;
		list->count = 0;
	}

	/* Set the capacity member to the new capacity. */
	list->capacity = newcap;

	return(list->capacity);

}
/* Shrinks the capacity of the list to the current count.
 *
 * If the count member has been corrupted, behavior is undefined.
 *
 * Returns:
 * 		0: Either the list count was 0, or a null pointer was passed.
 * 		The new capacity.
 */
size_t ArrayList_shrink(ArrayList* list)
{
	if(!list)
		return(0);
	else
		return(ArrayList_resize(list, list->count));
}

/* Sifts all the filled pointers to the first part of the array.
 * If the array was ordered before but had NULL pointers between
 * items, then the list will become unsorted.
 *
 * Parameters:
 * 		list: The list to sift.
 *
 * Returns:
 * 		alib_error: The error code associated with how the function ran.
 *
 * Note:
 * 		If ALIB_OBJ_CORRUPTION is returned, the ArrayList's count had been corrupted and is
 * 			automatically modified.  If this is returned, you should probably run some more
 * 			diagnostics on the object before continuing normal operations.
 */
alib_error ArrayList_sift(ArrayList* list)
{
	void** array_it;
	void** array_end_it;
	size_t array_count;

	if(!list)
		return(ALIB_BAD_ARG);

	if(list->count == 0)
		return(ALIB_OK);

	/* Ensure the first part of the list does not contain any NULL pointers. */
	array_it = list->list;
	array_end_it = list->list + list->capacity - 1;	//Point to the last item in the list.
	for(array_count = 0; array_count < list->count; ++array_count, ++array_it)
	{
		if(!*array_it)
		{
			for(; array_end_it != array_it && !*array_end_it; --array_end_it);

			/* This shouldn't be reached, but if it is, then the list has become corrupt.
			 * Try to handle it by modifying the array count. */
			if(array_end_it == array_it)
			{
				list->count = array_count;
				return(ALIB_OBJ_CORRUPTION);
			}

			*array_it = *array_end_it;
			*array_end_it = NULL;
		}
	}

	return(ALIB_OK);
}
/* Sorts the array list by calling 'qsort()' with the given comparison callback.
 *
 * Parameters:
 * 		list: The array list that should be sorted.
 * 		compare_cb: The callback function that is called to compare the values.
 *
 * Returns:
 * 		alib_error: Error code describing how the function ran.
 *
 * Note:
 * 		If ALIB_OBJ_CORRUPTION is returned, the ArrayList's count had been corrupted and is
 * 			automatically modified.  If this is returned, you should probably run some more
 * 			diagnostics on the object before continuing normal operations.
 */
alib_error ArrayList_sort(ArrayList* list, alib_compare_objects compare_cb)
{
	alib_error r_code;

	if(!list || !compare_cb)return(ALIB_BAD_ARG);

	/* Ensure the list is sifted. */
	r_code = ArrayList_sift(list);

	/* Sort the list of non-null pointers. */
	//qsort(list->list, list->count, sizeof(void*), (__compar_fn_t)compare_cb);
    qsort(list->list, list->count, sizeof(void*), (int(*)(const void*, const void*))compare_cb);
	return(r_code);
}

	/* Getters */
/* Returns the number of items are in the array.
 *
 * Assumes 'list' is not null. */
size_t ArrayList_get_count(ArrayList* list)
{
	return(list->count);
}
/* Returns the current capacity of the list.
 *
 * Assumes 'list' is not null. */
size_t ArrayList_get_capacity(ArrayList* list)
{
	return(list->capacity);
}
/* Returns the maximum capacity of the list.
 *
 * Assumes 'list' is not null. */
size_t ArrayList_get_max_capacity(ArrayList* list)
{
	return(list->max_cap);
}

/* Returns a pointer to the internal array.  The returned list
 * should only be read and never modified.
 *
 * Assumes 'list' is not null. */
const void** ArrayList_get_array_ptr(ArrayList* list)
{
	return((const void**)list->list);
}
/* Gets the item by the index.  The index is the virtual index of actual items
 * within the array instead of the actual array index.  To use the real index,
 * use the array pointer returned from 'ArrayList_get_array_ptr()'.
 *
 * Parameters:
 * 		list: The list to get the item from.
 * 		index: The index of the item to return.
 *
 * Returns:
 * 		A pointer to the item at the specified index or NULL if the
 * 		index is invalid or the object was corrupted.
 *
 * * Note:
 * 		It is not suggested to use this unless the object has first
 * 		been sorted as values aren't guaranteed nor are they static.
 */
const void* ArrayList_get_by_index(ArrayList* list, size_t v_index)
{
	void** array_it;
	size_t array_count;

	if(!list || v_index >= list->count)return(NULL);

	array_it = list->list;
	for(array_count = 0; array_count < list->capacity; ++array_count, ++array_it)
	{
		if(*array_it)
		{
			if(v_index == 0)
				return(*array_it);
			else
				--v_index;
		}
	}

	return(NULL);
}
/* Finds the list index of the item if it exists.  If it does not exist
 * in the list, then -1 is returned.
 *
 * Parameters:
 * 		list: The list to search in.
 * 		item: The item to search for.
 *
 * Returns:
 * 		>=0: The index of the item in the list.  This is the list index
 * 			not the actual array index.
 * 		-1: Item not found.
 * 		ALIB_BAD_ARG: As of this writing it is also -1, but is called
 * 			if a bad argument was passed. */
long ArrayList_get_item_index(ArrayList* list, const void* item)
{
	void** array_it;
	size_t array_count;
	size_t array_index;

	/* Check for bad arguments. */
	if(!list || !item)return(ALIB_BAD_ARG);

	/* Run through the list and find the 'list' index of the item. */
	array_it = list->list;
	array_index = 0;
	for(array_count = 0; array_count < list->capacity && array_index < list->count;
			++array_it)
	{
		if(*array_it)
		{
			if(*array_it == item)
				return(array_index);
			else
				++array_index;
		}
	}

	return(-1);
}
	/***********/

	/* Setters */
/* Sets the maximum capacity of the ArrayList to the given value.
 * If 'max_cap' is set to zero, then the maximum size will be SIZE_MAX.
 *
 * Assumes list is not null. */
void ArrayList_set_max_cap(ArrayList* list, size_t max_cap)
{
	if(max_cap == 0)
		max_cap = SIZE_MAX;

	list->capacity = max_cap;
}
	/***********/
/******************************/

/*******THREAD SAFE FUNCTIONS*******/
	/* PROTECTED FUNCTIONS */
/* Same as ArrayList_remove_by_ptr() but with mutexing. */
void ArrayList_remove_by_ptr_tsafe(ArrayList* list, void** item)
{
	if(!list || !item)
		return;

	if(list->use_mutex)
	{
		pthread_mutex_lock(&list->mutex);
		ArrayList_remove_by_ptr(list, item);
		pthread_mutex_unlock(&list->mutex);
	}
	else
		ArrayList_remove_by_ptr(list, item);
}
/* Same as ArrayList_remove_by_ptr_no_free() but with mutexing. */
void ArrayList_remove_by_ptr_no_free_tsafe(ArrayList* list, void** item)
{
	if(!list || !item)
		return;

	if(list->use_mutex)
	{
		pthread_mutex_lock(&list->mutex);
		ArrayList_remove_by_ptr_no_free(list, item);
		pthread_mutex_unlock(&list->mutex);
	}
	else
		ArrayList_remove_by_ptr_no_free(list, item);
}
	/***********************/

	/* PUBLIC FUNCTIONS */
/* Locks an array list.  Will block until the
 * list has been unlocked.
 *
 * If 'use_mutex' is 0, then this will do nothing.
 * Assumes 'list' is not null. */
void ArrayList_lock(ArrayList* list)
{
	if(list->use_mutex)
		pthread_mutex_lock(&list->mutex);
}
/* Unlocks an array list.
 *
 * If 'use_mutex' is 0, then this will do nothing.
 * Assumes 'list' is not null. */
void ArrayList_unlock(ArrayList* list)
{
	if(list->use_mutex)
		pthread_mutex_unlock(&list->mutex);
}

/* Same as ArrayList_add() but with mutexing. */
char ArrayList_add_tsafe(ArrayList* list, void* item)
{
	char r_val;

	if(!list)return(0);

	if(list->use_mutex)
	{
		pthread_mutex_lock(&list->mutex);
		r_val = ArrayList_add(list, item);
		pthread_mutex_unlock(&list->mutex);
	}
	else
		return(ArrayList_add(list, item));

	return(r_val);
}
/* Same as ArrayList_remove() but with mutexing. */
void ArrayList_remove_tsafe(ArrayList* list, void* item)
{
	if(!list)return;

	if(list->use_mutex)
	{
		pthread_mutex_lock(&list->mutex);
		ArrayList_remove(list, item);
		pthread_mutex_unlock(&list->mutex);
	}
	else
		ArrayList_remove(list, item);
}
/* Same as ArrayList_remove_no_free() but with mutexing. */
void ArrayList_remove_no_free_tsafe(ArrayList* list, void* item)
{
	if(!list || !item)
		return;

	if(list->use_mutex)
	{
		pthread_mutex_lock(&list->mutex);
		ArrayList_remove_no_free(list, item);
		pthread_mutex_unlock(&list->mutex);
	}
	else
		ArrayList_remove_no_free(list, item);
}
/* Same as ArrayList_get_first_item() but with mutexing. */
void* ArrayList_get_first_item_tsafe(ArrayList* list)
{
	/* Needed to store the value of ArrayList_get_first_item() so that
	 * it can be returned. */
	void* item;

	/* Check for errors. */
	if(!list)return(NULL);

	/* If mutexes are enabled, then lock and unlock the mutexes as
	 * needed. */
	if(list->use_mutex)
	{
		pthread_mutex_lock(&list->mutex);
		item = ArrayList_get_first_item(list);
		pthread_mutex_unlock(&list->mutex);

		return(item);
	}
	/* Otherwise just call ArrayList_get_first_item. */
	else
		return(ArrayList_get_first_item(list));
}
/* Same as ArrayList_find_item_by_value() but with mutexing. */
const void* ArrayList_find_item_by_value_tsafe(ArrayList* list, void* val, alib_compare_values compare_cb)
{
	const void* item;

	if(!list)return(NULL);

	if(list->use_mutex)
	{
		pthread_mutex_lock(&list->mutex);
		item = ArrayList_find_item_by_value(list, val, compare_cb);
		pthread_mutex_unlock(&list->mutex);
	}
	else
		item = ArrayList_find_item_by_value(list, val, compare_cb);

	return(item);
}

/* Same as ArrayList_clear() but with mutexing. */
void ArrayList_clear_tsafe(ArrayList* list)
{
	/* Check for errors. */
	if(!list)return;

	/* If mutexes are enabled, then lock and unlock the mutexes as
	 * needed. */
	if(list->use_mutex)
	{
		pthread_mutex_lock(&list->mutex);
		ArrayList_clear(list);
		pthread_mutex_unlock(&list->mutex);
	}
	/* Otherwise just call ArrayList_get_first_item. */
	else
		ArrayList_clear(list);
}

/* Same as ArrayList_resize() but with mutexing. */
size_t ArrayList_resize_tsafe(ArrayList* list, size_t newcap)
{
	size_t r_val;

	if(!list)return(0);

	if(list->use_mutex)
	{
		pthread_mutex_lock(&list->mutex);
		r_val = ArrayList_resize(list, newcap);
		pthread_mutex_unlock(&list->mutex);
	}
	else
		r_val = ArrayList_resize(list, newcap);
	return(r_val);
}
/* Same as ArrayList_shrink() but with mutexing. */
size_t ArrayList_shrink_tsafe(ArrayList* list)
{
	if(!list)
		return(0);
	else
		return(ArrayList_resize_tsafe(list, list->count));
}

/* Same as ArrayList_sift() but with mutexing. */
alib_error ArrayList_sift_tsafe(ArrayList* list)
{
	alib_error rval;

	if(!list)
		return(ALIB_BAD_ARG);

	if(list->use_mutex)
	{
		pthread_mutex_lock(&list->mutex);
		rval = ArrayList_sift(list);
		pthread_mutex_unlock(&list->mutex);
	}
	else
		rval = ArrayList_sift(list);

	return(rval);
}
/* Same as ArrayList_sort() but with mutexing. */
alib_error ArrayList_sort_tsafe(ArrayList* list, alib_compare_objects compare_cb)
{
	alib_error rval;

	if(!list)return(ALIB_BAD_ARG);

	if(list->use_mutex)
	{
		pthread_mutex_lock(&list->mutex);
		rval = ArrayList_sort(list, compare_cb);
		pthread_mutex_unlock(&list->mutex);
	}
	else
		rval = ArrayList_sort(list, compare_cb);

	return(rval);
}

		/* Getters */
/* Same as ArrayList_get_by_index() but with mutexing. */
const void* ArrayList_get_by_index_tsafe(ArrayList* list, size_t index)
{
	const void* rval;

	if(!list)return(NULL);

	if(list->use_mutex)
	{
		pthread_mutex_lock(&list->mutex);
		rval = ArrayList_get_by_index(list, index);
		pthread_mutex_unlock(&list->mutex);
	}
	else
		rval = ArrayList_get_by_index(list, index);

	return(rval);
}
/* Same as ArrayList_get_item_index() but with mutexing. */
long ArrayList_get_item_index_tsafe(ArrayList* list, const void* item)
{
	long rval;

	if(!list)return(ALIB_BAD_ARG);

	if(list->use_mutex)
	{
		pthread_mutex_lock(&list->mutex);
		rval = ArrayList_get_item_index(list, item);
		pthread_mutex_unlock(&list->mutex);
	}
	else
		rval = ArrayList_get_item_index(list, item);

	return(rval);
}
		/***********/

		/* Setters */
/* Changes the use of mutexing, but is unsafe if the mutex is currently being manipulated by
 * locking or unlocking. Only use this when you are sure no locking is being called on the mutex. */
void ArrayList_use_mutex(ArrayList* list, char use_mutex)
{
	if(!list || list->use_mutex == use_mutex)return;

	*((char*)&list->use_mutex) = use_mutex;

	if(list->use_mutex)
		pthread_mutex_init(&list->mutex, NULL);
	else
		pthread_mutex_destroy(&list->mutex);
}
		/***********/
	/********************/
/***********************************/

/*******CONSTRUCTORS*******/
/* Constructs a new ArrayList with the passed values.
 *
 * Parameters:
 * 		free_item: (OPTIONAL)A function callback used to free memory allocated by an item.  This should be called
 *		 					when the item is removed from the list.
 *		start_capacity: The initial capacity for the array.
 *		max_capacity: The maximum number of item pointers that the object may allocate.  If 0, then max_capacity
 *			will be set to UINT64_MAX.
 *		use_mutex: !0 will initialize mutexing and use it in *_tsafe() functions.
 *
 * Returns:
 * 		ArrayList*: New ArrayList.
 * 		NULL: Error occurred, either invalid values were passed or memory could not be
 * 			allocated.
 */
ArrayList* newArrayList_ex(alib_free_value free_item, size_t start_capacity,
		size_t max_capacity, char use_mutex)
{
	ArrayList* list;

	/* Check for valid capacity. */
	if(max_capacity == 0)
		max_capacity = SIZE_MAX;
	if(max_capacity < start_capacity)
		return(NULL);

	/* Allocate the object. */
	if(!(list = malloc(sizeof(ArrayList))))
		return(NULL);

	/* Initialize members. */
	list->capacity = start_capacity;
	list->count = 0;
	list->free_item = free_item;
	list->max_cap = max_capacity;

	*((char*)&list->use_mutex) = use_mutex;
	if(use_mutex)
		pthread_mutex_init(&list->mutex, NULL);

	/* Allocate list memory. We want all values to be NULL, so we use calloc. */
	list->list = calloc(list->capacity, sizeof(int*));

	return(list);
}
/* Constructs a new ArrayList with a max capacity of SIZE_MAX and an initial capacity of 4.
 * Mutexing is enabled by default.
 *
 * Parameters:
 * 		free_item: (OPTIONAL)A function callback used to free memory allocated by an item.  This should be called
 *		 					when the item is removed from the list.
 *
 * Returns:
 * 		ArrayList*: New ArrayList.
 * 		NULL: Error occurred, either invalid values were passed or memory could not be
 * 			allocated.
 */
ArrayList* newArrayList(alib_free_value free_item)
{
	return(newArrayList_ex(free_item, 4, 0, 1));
}
/* Frees all the memory allocated by the ArrayList and sets the pointer
 * to null.
 *
 * This call IS NOT THREADSAFE, attempting to destroy the object that is referenced
 * by multiple threads must be done very carefully.
 *
 * If a delitem_cb() was given, then it will be called on all the items in the
 * list.*/
void delArrayList(ArrayList** list)
{
	if(!list || !*list)return;

	/* Delete all the items in the list. */
	ArrayList_clear(*list);

	/* Destruct the mutex. */
	if((*list)->use_mutex)
		pthread_mutex_destroy(&(*list)->mutex);

	/* Delete the list. */
	free((*list)->list);
	free(*list);
	*list = NULL;
}
/**************************/
