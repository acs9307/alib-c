#ifndef ListHistoryItem_is_defined
#define ListHistoryItem_is_defined

#include "alib_error.h"

/* A list object which holds an item (void*) of any type.  When created
 * it will be placed in a singly-linked list.  Therefore, when destroyed,
 * the object at the front of the list is first destroyed.
 *
 * Should be created and destroyed by 'newListHistoryItem' and 'delListHistoryItem'
 * respectively.
 */
typedef struct ListHistoryItem
{
	/*Public Members*/
	/* The item placed in history.
	 * It is up to the user to remember what the item points to and how
	 * it should be handled.  Remember, a void* can point to pointers, so
	 * casting is extremely important when manipulating this member.
	 */
	void* list_item;

	/*Private Members*/
	/*The next(newer) ListHistory item in the list.*/
	struct ListHistoryItem* next;

	/*Returns a pointer to the item within the list.  If no item is found, returns NULL.*/
	struct ListHistoryItem*(*Find)(struct ListHistoryItem*, void*);
	/* Similar to 'find()' but finds the object before.  Used for list manipulation.
	 *
	 * Return Values:
	 * 		List before desired item: If a previous list is found, then it will be returned.
	 * 		NULL: If an error occurs or the item could not be found or there is nothing before the given
	 * 				given, returns NULL.
	 */
	struct ListHistoryItem*(*Find_One_Before)(struct ListHistoryItem*, void*);
	/* Removes the ListHistoryItem from the 'list' which matches the given 'item'.
	 *
	 * Return Codes:
	 * 		-1: Invalid Arguments
	 * 		 0: Item deleted.
	 * 		 1: Item not found, nothing deleted.
	 */
	char(*Remove)(struct ListHistoryItem**, void*);

	/*This function is called after the history item has been removed from the list.
	 *This should point to a function which specifies how the 'list_item' shall be destroyed.
	 *After the callback has been called, the ListHistory will be destroyed.
	 */
	void(*Remove_Callback)(void*);
	/* The callback function to compare two ListHistoryItems with each other.
	 * As the underlying functions do not know of the types, the return values
	 * will be read as stated below.
	 *
	 * This pointer should never be null, it should either point to a user specified function
	 * or to the default function.
	 *
	 * Return Values:
	 * 		 0: Both arguments are equal.
	 * 		!0: Arguments are not equal.
	 */
	char(*Compare_Callback)(void*, void*);
} ListHistoryItem;

/*Helper Functions*/
/* Frees any memory allocated by the ListHistoryItem */
void ListHistoryItem_helper_function_free(struct ListHistoryItem* list);

/* Default comparison function which compares two void pointers to items.
 * This is called if the ListHistoryItem->Compare_Callback is not set by the user.
 * This function assumes the given items are single pointers and will therefore
 * compare the pointer's address.
 */
char ListHistoryItem_helper_function_default_compare(void* item_1, void* item_2);
/******************/

/*Member Functions*/
/* Returns a pointer to the item within the list.  If no item is found, returns
 * NULL. */
ListHistoryItem* ListHistoryItem_member_function_find(ListHistoryItem* list,
		void* item);
/* Similar to 'find()' but finds the object before.  Used for list manipulation.
 *
 * Return Values:
 * 		List before desired item: If a previous list is found, then it will be
 * 			returned.
 * 		NULL: If an error occurs, 'item' could not be found, or nothing comes
 * 			before 'item' in the list.
 */
ListHistoryItem* ListHistoryItem_member_function_find_one_before(
		ListHistoryItem* list, void* item);

/* Removes the ListHistoryItem from the 'list' which matches the given 'item'.
 *
 * Return Codes:
 * 		-1: Invalid Arguments
 * 		 0: Item deleted.
 * 		 1: Item not found, nothing deleted.
 */
char ListHistoryItem_member_function_remove(struct ListHistoryItem** list,
		void* item);
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
alib_error newListHistoryItem(ListHistoryItem** list_hist, void* item,
		void(*remove_func)(void*), char(*compare_func)(void*,void*));

/* Destroy the top-most item, and set the pointer to point to the next object.
 * If this was the last item, it sets 'item' to NULL.
 */
void delListHistoryItem(ListHistoryItem** list);

/*Free the history object and all connected objects.*/
void ListHistoryItem_free_all(ListHistoryItem** item);
/******************************/
#endif
