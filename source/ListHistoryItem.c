#include <stdio.h>
#include <stdlib.h>

#include "ListHistoryItem.h"

/*Helper Functions*/
/* Frees any memory allocated by the ListHistoryItem */
void ListHistoryItem_helper_function_free(struct ListHistoryItem* list){free(list);}

/* Default comparison function which compares two void pointers to items.
 * This is called if the ListHistoryItem->Compare_Callback is not set by the user.
 * This function assumes the given items are single pointers and will therefore
 * compare the pointer's address.
 */
char ListHistoryItem_helper_function_default_compare(void* item_1, void* item_2)
{
	if(item_1 == item_2)
		return(0);
	else
		return(1);
}
/******************/

/*Member Functions*/
/* Returns a pointer to the item within the list.  If no item is found, returns
 * NULL. */
ListHistoryItem* ListHistoryItem_member_function_find(ListHistoryItem* list,
		void* item)
{
	if(!list || !item)return(NULL);

	//Search by iterating forward
	while(list && list->Compare_Callback(list->list_item, item))
		list = list->next;

	//It will either be pointing to the item or to NULL
	return(list);
}
/* Similar to 'find()' but finds the object before.  Used for list manipulation.
 *
 * Return Values:
 * 		List before desired item: If a previous list is found, then it will be
 * 			returned.
 * 		NULL: If an error occurs, 'item' could not be found, or nothing comes
 * 			before 'item' in the list.
 */
ListHistoryItem* ListHistoryItem_member_function_find_one_before(
		ListHistoryItem* list, void* item)
{
	if(!list || !item)return(NULL);

	//If we are already pointing to the desired item, return the item.
	if(list->Compare_Callback(list->list_item, item) == 0)
		return(NULL);

	//Search by iterating forward
	while(list->next && list->Compare_Callback(list->next->list_item, item) != 0)
		list = list->next;

	//It will either be pointing to the item before or to NULL
	if(list->next)
		return(list);
	else
		return(NULL);
}

/* Removes the ListHistoryItem from the 'list' which matches the given 'item'.
 *
 * Return Codes:
 * 		-1: Invalid Arguments
 * 		 0: Item deleted.
 * 		 1: Item not found, nothing deleted.
 */
char ListHistoryItem_member_function_remove(struct ListHistoryItem** list,
		void* item)
{
	ListHistoryItem* temp_list;

	if(!list || !*list || !item)return(-1);

	if((*list)->Compare_Callback((*list)->list_item, item) == 0)
	{
		delListHistoryItem(list);
		return(0);
	}
	else
	{
		temp_list = *list;

		//Peek ahead so that we can set the previous item's next to the deleted item's next
		while(temp_list->next && temp_list->Compare_Callback(temp_list->next->list_item, item) != 0)
			temp_list = temp_list->next;


		if(temp_list->next)
		{
			delListHistoryItem(&(temp_list->next));
			return(0);
		}
	}

	return(1);
}
/******************/

/*Constructors and Destructors*/
/* Creates a new ListHistory object.  If the list is empty, ensure that the list
 * pointer is pointing to NULL otherwise behavior is undefined.
 *
 * Arguments:
 * 		list_his: A pointer that will store the ListHistoryItem.  If other
 * 			items already exist in the list, then the new item will be added
 * 			to the end.
 * 		item: A pointer to the item which will be held by the ListHistoryItem.
 * 		remove_func: The function to be called to destroy the 'item' argument.
 * 			If NULL is passed, then this function will not be called.
 * 		compare_func: The function will be called if two items need to be compared.
 * 			If NULL is passed, the function will be set to the default ListHistoryItem
 * 			comparison function. Returning 0 means the two items are the same, anything
 * 			else means they are not equivalent.
 */
alib_error newListHistoryItem(ListHistoryItem** list_hist, void* item, void(*remove_func)(void*), char(*compare_func)(void*,void*))
{
	ListHistoryItem* temp_list;
	ListHistoryItem* list_it;

	if(!list_hist)return(ALIB_BAD_ARG);

	//Allocate memory
	temp_list = (ListHistoryItem*)malloc(sizeof(ListHistoryItem));
	if(!temp_list)return(ALIB_MEM_ERR);

	//Init public members
	temp_list->list_item = item;

	temp_list->Remove = ListHistoryItem_member_function_remove;
	temp_list->Find = ListHistoryItem_member_function_find;
	temp_list->Find_One_Before = ListHistoryItem_member_function_find_one_before;

	//Init private members
	temp_list->Remove_Callback = remove_func;
	temp_list->Compare_Callback = (compare_func)?compare_func:
			ListHistoryItem_helper_function_default_compare;
	temp_list->next = NULL;

	//Decide whether or not there is a history list already made
	if(*list_hist)
	{
		//Find end of list
		list_it = *list_hist;
		while(list_it->next){list_it = list_it->next;}

		//Link the new list to the previous lists
		list_it->next = temp_list;
	}
	else
		*list_hist = temp_list;

	return(ALIB_OK);
}

/* Destroy the top-most item, and set the pointer to point to the next object.
 * If this was the last item, it sets 'item' to NULL.
 */
void delListHistoryItem(ListHistoryItem** list)
{
	ListHistoryItem* temp_item;

	if(!list || !*list)return;

	temp_item = *list;
	*list = (*list)->next;

	//Call the callback function if available
	if(temp_item->Remove_Callback)
		temp_item->Remove_Callback(temp_item->list_item);

	//Free the item.
	ListHistoryItem_helper_function_free(temp_item);

	return;
}

/*Free the history object and all connected objects.*/
void ListHistoryItem_free_all(ListHistoryItem** item)
{
	if (!item)return;

	while(*item)
		delListHistoryItem(item);
}
/******************************/
