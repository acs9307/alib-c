#include <stdlib.h>
#include <inttypes.h>

#include "ListHistory.h"
#include "ListHistoryItem.h"
#include "MutexObject.h"

/*******Helper Functions*******/
/* Calculates the count.  This should only be used if the 'count' member was somehow corrupted.
 * If an error occurs, the function will return INT32_MAX, otherwise it set the 'hist->count'
 * and then return the same number.
 */
unsigned int ListHistory_helper_function_calc_count(ListHistory* hist)
{
	ListHistoryItem* temp_item;

	if(!hist)return(INT32_MAX);

	hist->count = 0;
	temp_item = hist->list_root;

	while(temp_item)
	{
		++(hist->count);
		temp_item = temp_item->next;
	}

	return(hist->count);
}

/******************************/

/*******Member Functions*******/
/* Removes the matching item from the history list.
 *
 * Return Codes:
 * 		LIST_HISTORY_MUTEX_ERR
 * 		LIST_HISTORY_BAD_ARG
 * 		LIST_HISTORY_OK
 */
char ListHistory_member_function_remove(ListHistory* hist, void* item)
{
	char r_code;
	ListHistoryItem* temp_item;

	if(!hist || !item)return(LIST_HISTORY_BAD_ARG);

#ifndef LIST_HISTORY_NO_MUTEX
	if(MutexObject_request_lock(hist->mutex))
		return(LIST_HISTORY_MUTEX_ERR);
#endif

	//Check the end pointer for the history, if it is the same as the given
	//item, then we need to set it to one place before the last item.
	if(hist->list_end->Compare_Callback(hist->list_end->list_item, item) == 0)
	{
		temp_item = hist->list_end->Find_One_Before(hist->list_root, hist->list_end->list_item);
		r_code = temp_item->Remove(&(temp_item->next), item);
		hist->list_end = temp_item;
	}
	else
		r_code= hist->list_root->Remove(&(hist->list_root), item);

	if(r_code == 0)
	{
		//If some error occurred and the history count was lost, recalculate it.
		if(!hist->count)
			ListHistory_helper_function_calc_count(hist);
		else
			--(hist->count);
	}

	//Do cleanup with the list pointers.
	if(!hist->list_end)
		hist->list_root = NULL;
	else if(!hist->list_root)
		hist->list_end = NULL;

#ifndef LIST_HISTORY_NO_MUTEX
	MutexObject_unlock(hist->mutex);
#endif

	return(LIST_HISTORY_OK);
}

/*Remove the top-most item from the list and update his->list_root.*/
void ListHistory_member_function_pop_off(ListHistory* hist)
{
	if(!hist)return;

#ifndef LIST_HISTORY_NO_MUTEX
	if(MutexObject_request_lock(hist->mutex))
		return;
#endif

	if(hist->list_end == hist->list_root)
		hist->list_end = NULL;

	if(hist->list_root)
	{
		delListHistoryItem(&(hist->list_root));
		//If some error occurred and the history count was lost, recalculate it.
		if(!hist->count)
			ListHistory_helper_function_calc_count(hist);
		else
			--(hist->count);
	}

#ifndef LIST_HISTORY_NO_MUTEX
	MutexObject_unlock(hist->mutex);
#endif
}

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
		char(*compare_func)(void*, void*))
{
	char r_code;

	if(!hist || !item)return(LIST_HISTORY_BAD_ARG);

#ifndef LIST_HISTORY_NO_MUTEX
	if(MutexObject_request_lock(hist->mutex))
		return(LIST_HISTORY_MUTEX_ERR);
#endif

	//Create a new Item
	r_code = newListHistoryItem(&(hist->list_end), item, remove_func, compare_func);
	if(r_code)
	{
#ifndef LIST_HISTORY_NO_MUTEX
		MutexObject_unlock(hist->mutex);
#endif
		return(r_code);
	}

	//If there was no list before, make root equal the end
	if(!(hist->list_root))
		hist->list_root = hist->list_end;
	//Otherwise increment the end pointer since we added to the end of the list.
	else
		hist->list_end = hist->list_end->next;

	++(hist->count);

#ifndef LIST_HISTORY_NO_MUTEX
	MutexObject_unlock(hist->mutex);
#endif

	if(hist->max > 0 && hist->count > hist->max)
		ListHistory_member_function_pop_off(hist);

	return(LIST_HISTORY_OK);
}

/* Move the 'hist->list_root' to 'hist->list_end'. (i.e. Make the oldest item
 * the newest.)*/
void ListHistory_member_function_move_top_to_bottom(ListHistory* hist)
{
	if(!hist)return;

#ifndef LIST_HISTORY_NO_MUTEX
	if(MutexObject_request_lock(hist->mutex))
		return;
#endif

	//Reshuffle the pointers.
	hist->list_end->next = hist->list_root;
	hist->list_root = hist->list_root->next;
	hist->list_end = hist->list_end->next;
	hist->list_end->next = NULL;

#ifndef LIST_HISTORY_NO_MUTEX
	MutexObject_unlock(hist->mutex);
#endif
}

/* Attempts to move the desired item to the top of the history list.
 *
 * Return Values:
 * 		LIST_HISTORY_MUTEX_ERR
 * 		LIST_HISTORY_BAD_ARG
 *		LIST_HISTORY_OK: Item successfully moved to top of list.
 * 		1: Item is not in list.
 */
char ListHistory_member_function_move_to_bottom(ListHistory* hist, void* item)
{
	ListHistoryItem* temp_item;

	if(!hist || !item)return(LIST_HISTORY_BAD_ARG);

#ifndef LIST_HISTORY_NO_MUTEX
	if(MutexObject_request_lock(hist->mutex))
		return(LIST_HISTORY_MUTEX_ERR);
#endif

	//Find the item before desired item in the history
	temp_item = hist->list_root->Find_One_Before(hist->list_root, item);

	//Item does not exist in the history
	if(!temp_item)
		return(1);
	//Item exists in the history, now move it to the bottom of the list
	else
	{
		//If we are not pointing to the desired item,
		//then the desired item must be next
		if(temp_item->Compare_Callback(temp_item->list_item, item) != 0)
		{
			hist->list_end->next = temp_item->next;
			temp_item->next = temp_item->next->next;
			hist->list_end = hist->list_end->next;
		}
		//The desired item is the oldest item, just move it to the bottom.
		else
			ListHistory_member_function_move_top_to_bottom(hist);
	}

#ifndef LIST_HISTORY_NO_MUTEX
	MutexObject_unlock(hist->mutex);
#endif

	return(LIST_HISTORY_OK);
}
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
char newListHistory(ListHistory** hist, int max)
{
	if(!hist)return(LIST_HISTORY_BAD_ARG);

	*hist = (ListHistory*)malloc(sizeof(ListHistory));
	if(!(*hist))return(LIST_HISTORY_MEM_ERR);

	(*hist)->Pop_Off = ListHistory_member_function_pop_off;
	(*hist)->Push_Back = ListHistory_member_function_push_back;
	(*hist)->Move_Top_to_Bottom = ListHistory_member_function_move_top_to_bottom;
	(*hist)->Move_to_Bottom = ListHistory_member_function_move_to_bottom;
	(*hist)->Remove = ListHistory_member_function_remove;

	(*hist)->count = 0;
	//The default value is (UINT32_MAX - 1) so that we don't have overflow when the
	//object has items added.
	(*hist)->max = (max)?max:INT32_MAX - 1;

#ifndef LIST_HISTORY_NO_MUTEX
	(*hist)->mutex = newMutexObject();
	if(!(*hist)->mutex)
	{
		free(*hist);
		return(LIST_HISTORY_MEM_ERR);
	}
#endif

	(*hist)->list_root = NULL;
	(*hist)->list_end = NULL;

	return(LIST_HISTORY_OK);
}

/* Deletes the ListHistory object and all it's children then sets the
 * pointer to null.*/
void delListHistory(ListHistory** hist)
{
	if(!hist || !(*hist))return;

#ifndef LIST_HISTORY_NO_MUTEX
	//Stop all mutexing, we are destroying the history object.
	delMutexObject(&(*hist)->mutex);
#endif

	if((*hist)->list_root)
		ListHistoryItem_free_all(&((*hist)->list_root));
	(*hist)->list_end = (*hist)->list_root;

	free((*hist));
	(*hist) = NULL;
}
/***********************/
