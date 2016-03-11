#ifndef DLIST_ITEM_IS_DEFINED
#define DLIST_ITEM_IS_DEFINED

#include <stdlib.h>
#include "alib_types.h"
#include "ListItem.h"
#include "ListItemVal.h"

/*******Class Declarations*******/
/* A doubly-linked list object.  It is named DListItem as it technically is just
 * a single item in the list, but a pointer to this object can, in fact, be
 * treated as a list.
 *
 * This is a minimalist object and therefore does not keep track of where the
 * beginning or end of the list is, nor does it keep track of how many items
 * are in the list, though it is capable of calculating all this value. */
typedef struct DListItem DListItem;
/********************************/

/*******Public Functions*******/
/* Inserts a DListItem into the provided 'list' after the given
 * 'list' pointer.
 *
 * Cannot add an item that already exists in a list, the item
 * must have a NULL parent.
 *
 * Parameters:
 * 		list: The list in which the new items should be inserted into.
 * 		new_item: The new DListItem that should be inserted into the
 * 			list.
 *
 * Returns:
 * 		'list': 'new_item' is not valid, either is NULL or already
 * 			belongs to a list (meaning 'parent' is not NULL).
 * 		'new_item': Successfully added 'new_item' to the list. */
DListItem* DListItem_insert_after(DListItem* list, DListItem* new_item);
/* Inserts a DListItem into the provided 'list' before the given
 * 'list' pointer.
 *
 * Cannot add an item that already exists in a list, the item
 * must have a NULL parent.
 *
 * Parameters:
 * 		list: The list in which the new item should be inserted into.
 * 		new_item: The new DListItem that should be inserted into the list.
 *
 * Returns:
 * 		'list': 'new_item' is not valid, either is NULL or already
 * 			belongs to a list (meaning 'parent' is not NULL).
 * 		'new_item': Successfully added 'new_item' to the list. */
DListItem* DListItem_insert_before(DListItem* list, DListItem* new_item);

/* Pushes an item onto the end of the list.
 * Here, end means the item with a NULL 'next' member.
 *
 * Cannot add an item that already exists in a list, the item
 * must have a NULL parent.
 *
 * Parameters:
 * 		list: The list to add to.
 * 		new_item: The item to add to the list.  This may also be a
 * 			pointer	to another list that is to be merged with 'list'.
 *
 * Returns:
 * 		Beginning of 'new_item':  Success.
 * 		'list': Either 'new_item' is null, or it already has a
 * 			parent. */
DListItem* DListItem_push_back(DListItem* list, DListItem* new_item);
/* Pushes an item to the beginning of the list.
 * Here, beginning means the item with a NULL 'prev' member.
 *
 * Cannot add an item that already exists in a list, the item
 * must have a NULL parent.
 *
 * Parameters:
 * 		list: The list to add to.
 * 		new_item: The item to add to the list.  This may also be a
 * 			pointer	to another list that is to be merged with 'list'.
 *
 * Returns:
 * 		Beginning of 'new_item':  Success.
 * 		'list': Either 'new_item' is null, or it already has a
 * 			parent. */
DListItem* DListItem_push_forward(DListItem* list, DListItem* new_item);

/* Moves an item from the 'from_index' to the 'to_index'.
 *
 * If 'to_index' exceeds LONG_MAX, behavior is undefined.
 *
 * Returns:
 * 		Success: The index that 'from_index' was placed.
 * 		Error: An alib_error code describing the error. */
long DListItem_move(DListItem* list, size_t from_index, size_t to_index);
/* Moves a group of items from the 'from_index' to the 'to_index'.
 * If the end of the list is hit before 'count' is reached, then only
 * the number of items found between 'from' and the end will be moved.
 *
 * If 'to_index' exceeds LONG_MAX, behavior is undefined.
 *
 * Returns:
 * 		Success: The index that 'from_index' was placed.
 * 		Error: An alib_error code describing the error. */
long DListItem_move_count(DListItem* list, size_t from_index,
		size_t to_index, size_t count);

/* Extracts an item from the list and updates the old list to reflect the
 * change.
 *
 * Parameters:
 * 		item: A double pointer to the item that is to be pulled out.  This
 * 			will be set to an adjacent item upon successful return. First
 * 			tries to move pointer to the previous item.
 *
 * Returns a pointer to the item that was pulled out of the list, or NULL on
 * error. */
DListItem* DListItem_pull_out(DListItem** item);
/* Similar to 'DListItem_pull_out()' but allows you to pull out more than
 * one item, therefore you can get a sub-list of the given list.
 *
 * WARNING -
 * 	If the end of the list is hit before 'count' is reached, only
 * 	the available number of items will be returned.  No warning or signal
 * 	is given, therefore the number of items pulled must be checked by the
 * 	caller.
 *
 * Parameters:
 * 		item: A double pointer to the item that is to be pulled out.  This
 * 			must be pointing to the first item in the sub list that is desired.
 * 			This will be set to an adjacent item upon successful return. First
 * 			tries to move pointer to the previous item.
 * 		count: The number of items to pull out after the 'item' pointer.  If
 * 			the end of the list is hit before 'count' is reached, then only
 * 			items up to the end will be pulled out.
 *
 * Returns a pointer to the item that was pulled out of the list, or NULL on
 * error. */
DListItem* DListItem_pull_out_count(DListItem** item, size_t count);

	/* Getters */
/* Gets the DListItem at the specified index. If the index
 * is greater than or equal to the length of the list, then
 * the last item in the list will be returned.  If 'index_exceeds'
 * is not null, then it will reflect the difference in the desired
 * index and the returned item's index.
 *
 * Parameters:
 * 		list: The list to get from.
 * 		index: The index of the item desired from the list.
 * 		index_exceeded (OUT): (OPTIONAL) The difference between the desired
 * 			index and the index found.
 *
 * Returns:
 * 		The item at the given index or the last item in the list. */
DListItem* DListItem_get(DListItem* list, size_t index, size_t* index_exceeds);
/* Returns the DListItem whose value's pointer address matches the given
 * value.  If searching for a NULL value, then the first NULL value found
 * will be returned.
 *
 * Parameters:
 * 		list: The list to search in for the value.
 * 		value: The value to search for.
 *
 * Returns:
 * 		A pointer to the DListItem who's value's pointer matches that of the
 * 		given 'value'. */
DListItem* DListItem_get_by_value(DListItem* list, const void* value);
/* Returns the DListItem at the given relative index. If the edge
 * of the list, either beginning or end, is hit before the index
 * is reached, the item at the edge will be returned.
 *
 * Relative index means the index relative to the given pointer.
 * For instance:
 * 		If 'list' points to an item in the middle of a list and you
 * 		called 'DListItem_get_by_relative_index(list, -2)', the item
 * 		two spots before 'list' would be returned.
 *
 * Parameters:
 * 		list: The list to get from.
 * 		index: The relative index of the item desired from the list.
 * 		index_exceeded (OUT): (OPTIONAL) The difference between the desired
 * 			index and the index found.
 *
 * Returns:
 * 		The item at the given index or the last item in the list. */
DListItem* DListItem_get_by_relative_index(DListItem* list, long index,
		long* index_exceeds);

/* Returns the number of items that are in the list based on a
 * single DListItem pointer.
 *
 * Parameters:
 * 		list: A DListItem pointer that belongs to the list whose
 * 			size should be calculated.
 *
 * Returns:
 * 		Number of DListItems found in the list. */
size_t DListItem_count(const DListItem* list);
/* Gets the index of the given DListItem.
 *
 * Returns:
 * 		>=0 - Index of the item in the list.
 * 		<0  - Error code. */
int DListItem_index(const DListItem* list);

/* Returns the first item in the list. */
DListItem* DListItem_get_first_item(const DListItem* list);
/* Returns the last item in the list. */
DListItem* DListItem_get_last_item(const DListItem* list);

/* Returns the next item in the list. */
DListItem* DListItem_get_next_item(const DListItem* list);
/* Returns the previous item in the list. */
DListItem* DListItem_get_prev_item(const DListItem* list);

/* Returns the list item's value.
 *
 * Assumes 'item' is not null. */
void* DListItem_get_value(const DListItem* item);
/* Returns a pointer to the item's ListItemVal object.
 *
 * Assumes 'item' is not null. */
ListItemVal* DListItem_get_value_object(const DListItem* item);
	/***********/

	/* Setters */
/* Sets the parent of the list for each item in the list. */
alib_error DListItem_set_list_parent(DListItem* list, void* parent);
	/***********/
/******************************/

/*******Constructors*******/
/* Creates a new DListItem object.  If 'old_list' is provided, the new
 * list item will be placed after the 'old_list's pointer position.
 *
 * Parameters:
 * 		item_value: The value that the item is associated with.
 * 		free_value_cb: The callback used to free the user provided value.
 * 		old_list: A pointer to an old list.  If this is provided, then the
 * 			new list item will be added after the given pointer.  Afterwards,
 * 			the pointer will be set to the new list item.
 *
 * Returns:
 * 		NULL: Error.
 * 		DListItem*: A newly instantiated DListItem. */
DListItem* newDListItem(void* item_value, alib_free_value free_value_cb,
		DListItem** old_list);
/* Deletes the given list item and sets the pointer to an adjacent
 * DListItem if possible (preferably the previous item).  If not,
 * the pointer will be set to NULL. */
void delDListItem(DListItem** list_item);

/* Calls delDListItem on all the DListItem objects in the list. */
void delDListItem_all(DListItem** list_item);
/**************************/



#endif
