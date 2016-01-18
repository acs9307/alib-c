#ifndef ListHistory_is_defined
#define ListHistory_is_defined

#include "ListHistoryItem.h"
#include "alib_error.h"

#ifndef LIST_HISTORY_NO_MUTEX
#include "MutexObject.h"
#endif


/* NOTES:
 * 		List history is, by default, mutexed. Though this is not suggested, you may
 * 		turn off mutexing by defining LIST_HISTORY_NO_MUTEX before compiling.
 */

/* RETURN CODES */
enum
{
	/* Mutexing error occurred. */
	LIST_HISTORY_MUTEX_ERR = ALIB_MUTEX_ERR,
	/* Unable to allocate memory. */
	LIST_HISTORY_MEM_ERR = ALIB_MEM_ERR,
	/* Invalid argument passed to function. */
	LIST_HISTORY_BAD_ARG = ALIB_BAD_ARG,
	/* Function completed successfully. */
	LIST_HISTORY_OK = ALIB_OK
};


/*Singly linked list object which contains history data for statements added to the cache.*/
typedef struct ListHistory
{
	/*Public Members*/
	/*Pointer to the oldest item in the list.*/
	ListHistoryItem* list_root;
	/*Pointer to the newest item in the list.*/
	ListHistoryItem* list_end;

	/*Number of items in the history list.*/
	unsigned int count;
	/*Maximum number of items allowed in the list.*/
	int max;

	/* Object used for mutexing. */
	struct MutexObject* mutex;

	/*Remove the top-most item from the list and update his->list_root.*/
	void (*Pop_Off)(struct ListHistory*);
	/* Adds an item to the history list, if the list become larger than the maximum, it will call 'hist->Pop_Off'.
	 *
	 * Parameters:
	 * 		'hist': This history object that will handle the new item.
	 * 		'item': The item to be handled.
	 * 		'remove_func': (OPTIONAL) The function to call when removing
	 * 				an item from history.
	 * 		'compare_func': (OPTIONAL-SUGGESTED) The function to call when trying
	 * 				to compare two items.  Highly suggested to provide
	 * 				a comparison function, particularly if you will ever
	 * 				use Remove() or Move_to_Bottom(). Returning 0 means both
	 * 				items are equal, anything else means they aren't.
	 *
	 * 		All optional functions should be set to NULL if not used.
	 *
	 * Return Values:
	 * 		-2: Cannot allocate memory.
	 * 		-1: Unknown error.
	 * 		 0: No error.
	 */
	char (*Push_Back)(struct ListHistory*, void*, void(*remove_func)(void*), char(*compare_func)(void*,void*));
	/* Move the 'hist->list_root' to 'hist->list_end'. (i.e. Make the oldest item the newest.)*/
	void (*Move_Top_to_Bottom)(struct ListHistory*);
	/* Attempts to move the desired item to the top of the history list, (i.e. newest position).
	 *
	 * Return Values:
	 * 		-1: Unknown error, perhaps null pointers were passed.
	 * 		 0: Item successfully moved to top of list.
	 * 		 1: Item is not in list.
	 */
	char (*Move_to_Bottom)(struct ListHistory*, void*);
	/* Removes the matching item from the history list.
	 *
	 * Return Codes:
	 * 		-1: Invalid arguments.
	 * 		 0: Item removed successfully.
	 */
	char (*Remove)(struct ListHistory*, void*);

} ListHistory;

/*******Helper Functions*******/
/* Calculates the count.  This should only be used if the 'count' member was somehow corrupted.
 * If an error occurs, the function will return INT32_MAX, otherwise it set the 'hist->count'
 * and then return the same number.
 */
unsigned int ListHistory_helper_function_calc_count(ListHistory* hist);

/******************************/

/*******Member Functions*******/
/* Removes the matching item from the history list.
 *
 * Return Codes:
 * 		LIST_HISTORY_MUTEX_ERR
 * 		LIST_HISTORY_BAD_ARG
 * 		LIST_HISTORY_OK
 */
char ListHistory_member_function_remove(ListHistory* hist, void* item);

/*Remove the top-most item from the list and update his->list_root.*/
void ListHistory_member_function_pop_off(ListHistory* hist);

/* Adds an item to the history list, if the list become larger than the maximum,
 * it will call 'hist->Pop_Off'.
 *
 * Parameters:
 * 		'hist': This history object that will handle the new item.
 * 		'item': The item to be handled.
 * 		'remove_func': (OPTIONAL) The function to call when removing
 * 				an item from history.
 * 		'compare_func': (OPTIONAL-SUGGESTED) The function to call when trying
 * 				to compare two items.  Highly suggested to provide
 * 				a comparison function, particularly if you will ever
 * 				use Remove() or Move_to_Bottom(). Returning 0 means both
 * 				items are equal, anything else means they aren't.
 *
 * 		All optional functions should be set to NULL if not used.
 *
 * Return Values:
 * 		LIST_HISTORY_MUTEX_ERR
 * 		LIST_HISTORY_MEM_ERR
 * 		LIST_HISTORY_BAD_ARG
 * 		LIST_HISTORY_OK
 */
char ListHistory_member_function_push_back(ListHistory* hist, void* item, void(*remove_func)(void*),
		char(*compare_func)(void*, void*));

/* Move the 'hist->list_root' to 'hist->list_end'. (i.e. Make the oldest item
 * the newest.)*/
void ListHistory_member_function_move_top_to_bottom(ListHistory* hist);

/* Attempts to move the desired item to the top of the history list.
 *
 * Return Values:
 * 		LIST_HISTORY_MUTEX_ERR
 * 		LIST_HISTORY_BAD_ARG
 *		LIST_HISTORY_OK: Item successfully moved to top of list.
 * 		1: Item is not in list.
 */
char ListHistory_member_function_move_to_bottom(ListHistory* hist, void* item);
/******************************/

/*******Lifecycle*******/
/* Creats a new ListHistory object with a max list size of the give 'max'.
 * If 'max' is set to zero, then the maximum will be the default value
 * (INT32_MAX - 1).  If 'max' is set to <0 it will turn off auto deletion of
 * history items. 'hist' should be uninitialized or deleted before being passed,
 * otherwise there will be a memory leak.
 *
 * Return Values:
 * 		LIST_HISTORY_OK
 * 		LIST_HISTORY_BAD_ARG
 * 		LIST_HISTORY_MEM_ERR
 */
char newListHistory(ListHistory** hist, int max);

/* Deletes the ListHistory object and all it's children then sets the
 * pointer to null.*/
void delListHistory(ListHistory** hist);
/***********************/

#endif
