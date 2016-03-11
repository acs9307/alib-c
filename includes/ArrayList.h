#ifndef ARRAY_LIST_IS_DEFINED
#define ARRAY_LIST_IS_DEFINED

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <limits.h>
#include <pthread.h>
#include <string.h>

#include "alib_types.h"
#include "alib_error.h"

/* A list that stores data in an array.  This is useful when items need to
 * constantly be added to and removed from the list as it does not have to allocate
 * memory each time an item is added.
 *
 * When adding items to the list, searching starts at the beginning of the array and
 * goes forward until it finds a NULL pointer, therefore it is NOT useful if order
 * of items is important.
 *
 * Allocated memory doubles for each resize and will NOT automatically shrink.
 *
 * The array holding the items is a 2d array, meaning that all items must be pointers
 * to allocated memory, if not, behavior is undefined.
 *
 * To turn on or off mutexing, define ARRAY_LIST_USE_MUTEX as !0 or 0 respectively.
 * Mutexing is turned on by default.
 */
typedef struct ArrayList ArrayList;

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
char ArrayList_add(ArrayList* list, void* item);
/* Removes an item from the list by searching for an item pointing to the same
 * memory address. If the item is found it will be removed from the list, but
 * the delete callback will not be called on it.
 *
 * Parameters:
 * 		list: The list to remove the item from.
 * 		item: The item that should be removed. */
void ArrayList_remove_no_free(ArrayList* list, void* item);
/* Removes an item from the list by searching for an item pointing to the same
 * memory address. If the item is found, it will be removed from the list
 * and the delete callback will be called on it.
 *
 * Parameters:
 * 		list: The list to remove the item from.
 * 		item: The item that should be removed. */
void ArrayList_remove(ArrayList* list, void* item);
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
void* ArrayList_get_first_item(const ArrayList* list);
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
const void* ArrayList_find_item_by_value(const ArrayList* list, const void* val, alib_compare_values compare_cb);
/* Returns true if 'data' matches an item in 'list' */
char ArrayList_contains(const ArrayList* list, const void* data, size_t data_len);

/* Removes all the items from the list without actually deleting the list.
 *
 * Parameters:
 * 		list: The ArrayList to clear. */
void ArrayList_clear(ArrayList* list);

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
size_t ArrayList_resize(ArrayList* list, size_t newcap);
/* Shrinks the capacity of the list to the current count.
 *
 * If the count member has been corrupted, behavior is undefined.
 *
 * Returns:
 * 		0: Either the list count was 0, or a null pointer was passed.
 * 		The new capacity.
 */
size_t ArrayList_shrink(ArrayList* list);

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
alib_error ArrayList_sift(ArrayList* list);
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
alib_error ArrayList_sort(ArrayList* list, alib_compare_objects compare_cb);

/* Extracts the internal array and returns it.
 * After extraction, the list neither owns nor knows of the
 * array, therefore the user MUST FREE THE MEMORY pointed to by
 * the return value. */
void** ArrayList_extract_array(ArrayList* list);

	/* Getters */
/* Returns the number of items are in the array.
 *
 * Assumes 'list' is not null. */
size_t ArrayList_get_count(const ArrayList* list);
/* Returns the current capacity of the list.
 *
 * Assumes 'list' is not null. */
size_t ArrayList_get_capacity(const ArrayList* list);
/* Returns the maximum capacity of the list.
 *
 * Assumes 'list' is not null. */
size_t ArrayList_get_max_capacity(const ArrayList* list);

/* Returns a pointer to the internal array.  The returned list
 * should only be read and never modified.
 *
 * Assumes 'list' is not null. */
const void** ArrayList_get_array_ptr(const ArrayList* list);
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
const void* ArrayList_get_by_index(const ArrayList* list, size_t v_index);
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
long ArrayList_get_item_index(const ArrayList* list, const void* item);
	/***********/

	/* Setters */
/* Sets the maximum capacity of the ArrayList to the given value.
 * If 'max_cap' is set to zero, then the maximum size will be ULONG_MAX.
 *
 * Assumes list is not null. */
void ArrayList_set_max_cap(ArrayList* list, size_t max_cap);
	/***********/
/******************************/

/*******THREAD SAFE FUNCTIONS*******/
	/* PUBLIC FUNCTIONS */
/* Locks an array list.  Will block until the
 * list has been unlocked.
 *
 * If 'use_mutex' is 0, then this will do nothing.
 * Assumes 'list' is not null. */
void ArrayList_lock(ArrayList* list);
/* Unlocks an array list.
 *
 * If 'use_mutex' is 0, then this will do nothing.
 * Assumes 'list' is not null. */
void ArrayList_unlock(ArrayList* list);

/* Same as ArrayList_add() but with mutexing. */
char ArrayList_add_tsafe(ArrayList* list, void* item);
/* Same as ArrayList_remove() but with mutexing. */
void ArrayList_remove_tsafe(ArrayList* list, void* item);
/* Same as ArrayList_remove_no_free() but with mutexing. */
void ArrayList_remove_no_free_tsafe(ArrayList* list, void* item);
/* Same as ArrayList_find_item_by_value() but with mutexing. */
const void* ArrayList_find_item_by_value_tsafe(ArrayList* list, void* val, alib_compare_values compare_cb);

/* Same as ArrayList_clear() but with mutexing. */
void ArrayList_clear_tsafe(ArrayList* list);

/* Same as ArrayList_resize() but with mutexing. */
size_t ArrayList_resize_tsafe(ArrayList* list, size_t newcap);
/* Same as ArrayList_shrink() but with mutexing. */
size_t ArrayList_shrink_tsafe(ArrayList* list);

/* Same as ArrayList_sift() but with mutexing. */
alib_error ArrayList_sift_tsafe(ArrayList* list);
/* Same as ArrayList_sort() but with mutexing. */
alib_error ArrayList_sort_tsafe(ArrayList* list, alib_compare_objects compare_cb);

/* Same as ArrayList_extract_array() but with mutexing. */
void** ArrayList_extract_array_tsafe(ArrayList* list);

		/* Getters */
/* Same as ArrayList_get_first_item() but with mutexing. */
void* ArrayList_get_first_item_tsafe(ArrayList* list);
/* Same as ArrayList_get_by_index() but with mutexing. */
const void* ArrayList_get_by_index_tsafe(ArrayList* list, size_t index);
/* Same as ArrayList_get_item_index() but with mutexing. */
long ArrayList_get_item_index_tsafe(ArrayList* list, const void* item);
		/***********/

		/* Setters */
/* Changes the use of mutexing, but is unsafe if the mutex is currently being manipulated by
 * locking or unlocking. Only use this when you are sure no locking is being called on the mutex. */
void ArrayList_use_mutex(ArrayList* list, char use_mutex);
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
		size_t max_capacity, char use_mutex);
/* Constructs a new ArrayList with a max capacity of ULONG_MAX and an initial capacity of 4.
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
ArrayList* newArrayList(alib_free_value free_item);

/* Frees all the memory allocated by the ArrayList and sets the pointer
 * to null.
 *
 * This call IS NOT THREADSAFE, attempting to destroy the object that is referenced
 * by multiple threads must be done very carefully.
 *
 * If a delitem_cb() was given, then it will be called on all the items in the
 * list.*/
void delArrayList(ArrayList** list);
/**************************/


#endif
