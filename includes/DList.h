#ifndef DLIST_IS_DEFINED
#define DLIST_IS_DEFINED

#include <stdlib.h>

#include "alib_error.h"
#include "DListItem.h"
#include "ListItem.h"

typedef struct DList DList;

/*******Public Functions*******/
/* Adds a DListItem object to the end of the list.
*
* Parameters:
* 		'list': The list to push back onto.
* 		'item': The item to push back into the list.
* 			MUST not have a parent. */
alib_error DList_push_back(DList* list, DListItem* item);
/* Adds a DListItem object to the front of the list.
*
* Parameters:
* 		'list': The list to push back onto.
* 		'item': The item to push back into the list.
* 			MUST not have a parent. */
alib_error DList_push_forward(DList* list, DListItem* item);

/* Inserts an item at at the given index.
*
* Parameters:
* 		'list': The list to push back onto.
* 		'index': The index to add the item at.
* 		'item': The item to push back into the list.
* 			MUST not have a parent. */
alib_error DList_insert(DList* list, size_t index, DListItem* item);
/* Inserts an item after the given item.
 *
 * Parameters:
 *             list: The list to modify.
 *             curItm: The item to insert after.  This item's parent MUST BE 'list'.
 *             newItm: The item to insert.  This item's parent MUST BE NULL. */
alib_error DList_insert_after(DList* list, DListItem* curItm, DListItem* newItm);
/* Inserts an item before the given item.
 *
 * Parameters:
 *             list: The list to modify.
 *             curItm: The item to insert before.  This item's parent MUST BE 'list'.
 *             newItm: The item to insert.  This item's parent MUST BE NULL. */
alib_error DList_insert_before(DList* list, DListItem* curItm, DListItem* newItm);

/* Moves an item from a specific index to another index.
*
* If 'to' surpasses the length of the list, then 'from' will
* be placed at the end of the list.
*
* Parameters:
* 		'list': The list to modify.
* 		'from_index': The index to move from.
* 		'to_index': The index to move to.
*
* Returns:
* 		<0: Alib error code.
* 		>=0: Index that the item at 'from_index' was moved to. */
long DList_move(DList* list, size_t from_index, size_t to_index);
/* Moves a group of items starting at a specific index to a given index.
*
* If 'to' surpasses the length of the list, then 'from' will
* be placed at the end of the list.
*
* Parameters:
* 		list: The list to modify.
* 		from_index: The index to move from.
* 		to_index: The index to move to.
* 		count: The number of items to move.  If number of items
* 			past 'from_index' is less than 'count', then only the
* 			number of items found will be moved.
*
* Returns:
* 		<0: Alib error code.
* 		>=0: Index that the item at 'from_index' was moved to. */
long DList_move_count(DList* list, size_t from_index, size_t to_index,
	size_t count);

/* Removes an item at the given index.
*
* Parameters:
* 		list: The list to modify.
* 		index: The index to remove. */
alib_error DList_remove(DList* list, size_t index);
/* Marks the item at the given index for removal.  Safe to use in loops.
*
* Parameters:
*		list: The list to remove from.
*		index: The index of the item to remove. */
alib_error DList_remove_lsafe(DList* list, size_t index);
/* Removes a set of items at the given index.
*
* Parameters:
* 		list: The list to modify.
* 		index: The index to remove.
* 		count: The number of items to remove.  If the count exceeds
* 			the number of items found after 'index', then only the
* 			found items will be removed. */
alib_error DList_remove_count(DList* list, size_t index, size_t count);
/* Similar to 'DList_remove_count()' except that the items are only marked for removal rather than actually removed.
*
* Parameters:
* 		list: The list to modify.
* 		index: The index to remove.
* 		count: The number of items to remove.  If the count exceeds
* 			the number of items found after 'index', then only the
* 			found items will be removed. */
alib_error DList_remove_count_lsafe(DList* list, size_t index, size_t count);
/* Removes an item from the list.
*
* Parameters:
* 		list: The list to modify.
* 		item: The item to remove from the list.  The item MUST
* 			belong to 'list'.  If it does not, then the function
* 			will fail. */
alib_error DList_remove_item(DList* list, DListItem* item);
/* Marks an item to be removed from the list.
*
* Parameters:
* 		list: The list to modify.
* 		item: The item to remove from the list.  The item MUST
* 			belong to 'list'.  If it does not, then the function
* 			will fail. */
alib_error DList_remove_item_lsafe(DList* list, DListItem* item);
/* Removes 'count' number of items starting at 'item'.
*
* Parameters:
* 		list: The list to modify.
* 		item: The beginning of the set of items to remove from the list.
*			The item MUST belong to 'list'.  If it does not, then
*			the function will fail.
* 		count: The number of items to remove from the list.  If the
* 			end of the list is hit before the count is reached, then
* 			only the found items will be removed.  */
alib_error DList_remove_item_count(DList* list, DListItem* item, size_t count);
/* Marks 'count' number of items starting at 'item' for removal.
*
* Parameters:
* 		list: The list to modify.
* 		item: The beginning of the set of items to remove from the list.
*			The item MUST belong to 'list'.  If it does not, then
*			the function will fail.
* 		count: The number of items to remove from the list.  If the
* 			end of the list is hit before the count is reached, then
* 			only the found items will be removed.  */
alib_error DList_remove_item_count_lsafe(DList* list, DListItem* item, size_t count);

/* Removes all items that have been marked for removal.  This should be called whenever it is safe to remove
 * items that were removed by '_lsafe()' functions. */
void DList_remove_marked_items(DList* list);

/* Pulls an item from the list at the given index and returns it.
*
* Parameters:
* 		list: The list to modify.
* 		index: The index to pull from.
*
* Returns:
* 		NULL: Error.
* 		DListItem*: The pointer to the pulled item. */
DListItem* DList_pull_out(DList* list, size_t index);
/* Pulls a set of items from the list starting at the given index.
* If the end of the list is reached before 'count' is, then only
* the found items will be pulled.
*
* Parameters:
* 		list: The list to modify.
* 		index: The index to pull from.
* 		count: The (maximum) number of items to remove from the list.
* 			If the end of the list is hit first, the returned items
* 			may be fewer than 'count'.
*
* Returns:
* 		NULL: Error.
* 		DListItem*: A pointer to the items pulled. */
DListItem* DList_pull_out_count(DList* list, size_t index, size_t count);

	/* Getters */
/* Returns the number of items in the list.
*
* Assumes 'list' is not null. */
size_t DList_get_count(const DList* list);

/* Returns the item at the given index.
*
* Returns:
* 		NULL: Error, list is null or index is out of range.
* 		ListItem*: The item found at the given index. */
ListItem* DList_get(DList* list, size_t index);
/* Returns the first item that has a value whose pointer points to the same
* memory location as 'val'.
*
* Returns:
* 		NULL: Error, no matching value.
* 		DListItem*: A pointer to the item found. */
ListItem* DList_get_by_value(DList* list, void* val);

/* Returns the first item in the list. */
const DListItem* DList_get_begin(const DList* list);
/* Returns the last item in the list. */
const DListItem* DList_get_end(const DList* list);
	/***********/

	/* Lifecycle */
/* Default constructor. */
DList* newDList();
/* Default destructor. */
void delDList(DList** list);
	/*************/
/******************************/

#endif
