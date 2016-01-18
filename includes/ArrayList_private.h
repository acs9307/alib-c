#ifndef ARRAY_LIST_PRIVATE_IS_DEFINED
#define ARRAY_LIST_PRIVATE_IS_DEFINED

#include "ArrayList_protected.h"

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
struct ArrayList
{
	/* The raw array list of items.*/
	void** list;
	/* The number of items in the array. */
	size_t count;
	/* The total number of items that can be stored before reallocation
	 * is required. */
	size_t capacity;

	/* The maximum number of items that will be stored in the list. */
	size_t max_cap;

	/* A function pointer to the function which frees memory of an item. */
	alib_free_value free_item;

	/* If !0, mutexing is used, otherwise mutexing is not used.  This should
	 * only be set within the constructor and should not be modified elsewhere. */
	const char use_mutex;
	pthread_mutex_t mutex;
};

#endif
