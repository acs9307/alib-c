#include "DList_private.h"
#include "DListItem_private.h"

/*******Public Functions*******/
/* Adds a DListItem object to the end of the list.
 *
 * Parameters:
 * 		'list': The list to push back onto.
 * 		'item': The item to push back into the list.
 * 			MUST not have a parent. */
alib_error DList_push_back(DList* list, DListItem* item)
{
	if(!list || !item || item->base.parent)return(ALIB_BAD_ARG);

	size_t item_len = DListItem_count(item);
	list->end = DListItem_push_back(list->end, item);
	list->count += item_len;

	/* If there were no items in the list before this call
	 * then set the beginning pointer to the end pointer as
	 * it will be pointing to the beginning of 'item'. */
	if(!list->begin)list->begin = list->end;

	list->end = DListItem_get_last_item(list->end);
	if(!list->begin->base.parent)
		DListItem_set_list_parent(list->begin, list);

	return(ALIB_OK);
}
/* Adds a DListItem object to the front of the list.
 *
 * Parameters:
 * 		'list': The list to push back onto.
 * 		'item': The item to push back into the list.
 * 			MUST not have a parent. */
alib_error DList_push_forward(DList* list, DListItem* item)
{
	if(!list || !item || item->base.parent)return(ALIB_BAD_ARG);

	size_t item_len = DListItem_count(item);
	list->begin = DListItem_push_forward(list->begin, item);
	list->count += item_len;

	/* Remember to update the 'end' pointer if there was nothing
	 * in the list before. */
	if(!list->end)list->end = DListItem_get_last_item(list->begin);
	if(!list->begin->base.parent)
			DListItem_set_list_parent(list->begin, list);

	return(ALIB_OK);
}

/* Inserts an item at at the given index.
 *
 * Parameters:
 * 		'list': The list to push back onto.
 * 		'index': The index to add the item at.
 * 		'item': The item to push back into the list.
 * 			MUST not have a parent. */
alib_error DList_insert(DList* list, size_t index, DListItem* item)
{
	if(!list || !item || item->base.parent)return(ALIB_BAD_ARG);

	/* Calculate the size of 'item' before modifying the list
	 * as 'item' will become part of list and calculating the count
	 * afterwards will give the entire list's count. */
	size_t item_count = DListItem_count(item);

	if(!list->begin)
	{
		list->begin = DListItem_get_first_item(item);
		list->end = DListItem_get_last_item(item);
	}
	else if(index >= list->count)
	{
		DListItem_insert_after(list->end, item);
		list->end = DListItem_get_last_item(list->end);
	}
	else if(index < list->count / 2)
	{
		DListItem_insert_before(
			DListItem_get_by_relative_index(list->begin, index, NULL),
			item);
	}
	else
	{
		DListItem_insert_before(
			DListItem_get_by_relative_index(list->end, index - (list->count - 1), NULL),
			item);
	}

	if(!list->begin->base.parent)
		DListItem_set_list_parent(list->begin, list);
	list->count += item_count;
	return(ALIB_OK);
}

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
long DList_move(DList* list, size_t from_index, size_t to_index)
{
	if(!list)return(ALIB_BAD_ARG);
	if(from_index == to_index || list->count == 1)
		return(from_index);

	if(from_index == 0)
		list->begin = list->begin->next;
	if(from_index == list->count - 1)
		list->end = list->end->prev;

	from_index = DListItem_move(list->begin, from_index, to_index);

	if(to_index == 0)
		list->begin = DListItem_get_first_item(list->begin);
	else if(to_index >= list->count)
		list->end = DListItem_get_last_item(list->end);

	return((long)from_index);
}
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
		size_t count)
{
	if(!list)return(ALIB_BAD_ARG);

	from_index = DListItem_move_count(list->begin, from_index, to_index,
			count);

	/* Though I could try to shuffle the beginning and ending pointers around
	 * like I did in 'DList_move_count()', I believe the calculations required
	 * may outweigh the benefit, so I simply just iterate them back to the
	 * beginning and end as needed. */
	list->begin = DListItem_get_first_item(list->begin);
	list->end = DListItem_get_last_item(list->end);

	return((long)from_index);
}

/* Removes an item at the given index.
 *
 * Parameters:
 * 		list: The list to modify.
 * 		index: The index to remove. */
alib_error DList_remove(DList* list, size_t index)
{
	if(!list || index >= list->count)return(ALIB_BAD_ARG);

	/* Beginning */
	if(index == 0)
	{
		delDListItem(&list->begin);
		if(!list->begin)list->end = NULL;
	}
	/* End - Don't need to check to see if 'end' is null since that would mean
	 * that there was nothing else in the list and therefore the given index
	 * would be 0. */
	else if(index == list->count - 1)
		delDListItem(&list->end);
	/* Front half. */
	else if(index < list->count / 2)
	{
		DListItem* dli = DListItem_get_by_relative_index(list->begin, index, NULL);
		delDListItem(&dli);
	}
	/* Back half. */
	else
	{
		DListItem* dli = DListItem_get_by_relative_index(list->end,
				index - (list->count - 1), NULL);
		delDListItem(&dli);
	}

	--list->count;
	return(ALIB_OK);
}
/* Removes a set of items at the given index.
 *
 * Parameters:
 * 		list: The list to modify.
 * 		index: The index to remove.
 * 		count: The number of items to remove.  If the count exceeds
 * 			the number of items found after 'index', then only the
 * 			found items will be removed. */
alib_error DList_remove_count(DList* list, size_t index, size_t count)
{
	if(!list || index >= list->count)return(ALIB_BAD_ARG);

	/* Pull out a list then delete them. */
	DListItem* remove_list = DList_pull_out_count(list,
			index, count);

	delDListItem_all(&remove_list);

	return(ALIB_OK);
}
/* Removes an item from the list.
 *
 * Parameters:
 * 		list: The list to modify.
 * 		item: The item to remove from the list.  The item MUST
 * 			belong to 'list'.  If it does not, then the function
 * 			will fail. */
alib_error DList_remove_item(DList* list, DListItem* item)
{
	if(!list || item->base.parent != list)return(ALIB_BAD_ARG);

	if(item == list->begin)
	{
		delDListItem(&list->begin);
		if(!list->begin)list->end = NULL;
	}
	else if(item == list->end)
		delDListItem(&list->end);
	else
		delDListItem(&item);

	--list->count;
	return(ALIB_OK);
}
/* Removes a set of items from the list.
 *
 * Parameters:
 * 		list: The list to modify.
 * 		item: The item to remove from the list.  The item MUST
 * 			belong to 'list'.  If it does not, then the function
 * 			will fail.
 * 		count: The number of items to remove from the list.  If the
 * 			end of the list is hit before the count is reached, then
 * 			only the found items will be removed.  */
alib_error DList_remove_item_count(DList* list, DListItem* item, size_t count)
{
	/* We have to calculate the index of the item, otherwise we can't
	 * determine the number of items actually removed, therefore we just
	 * use the 'DList_remove_count()' function.  */
	return(DList_remove_count(list, DListItem_index(item), count));
}

/* Pulls an item from the list at the given index and returns it.
 *
 * Parameters:
 * 		list: The list to modify.
 * 		index: The index to pull from.
 *
 * Returns:
 * 		NULL: Error.
 * 		DListItem*: The pointer to the pulled item. */
DListItem* DList_pull_out(DList* list, size_t index)
{
	if(!list || index >= list->count)return(NULL);

	DListItem* rval;
	if(index == 0)
	{
		rval = DListItem_pull_out(&list->begin);
		if(!list->begin)list->end = NULL;
	}
	else if(index == list->count - 1)
		rval = DListItem_pull_out(&list->end);
	else
	{
		if(index < list->count / 2)
			rval = DListItem_get_by_relative_index(list->begin, index, NULL);
		else
			rval = DListItem_get_by_relative_index(list->end,
					index - (list->count - 1), NULL);
		rval = DListItem_pull_out(&rval);
	}

	--list->count;
	return(rval);
}
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
DListItem* DList_pull_out_count(DList* list, size_t index, size_t count)
{
	if(!list)return(NULL);

	DListItem* rval;
	if(index == 0)
	{
		rval = DListItem_pull_out_count(&list->begin, count);
		if(!list->begin)list->end = NULL;
	}
	else if(index == list->count - 1)
		rval = DListItem_pull_out_count(&list->end, count);
	else
	{
		if(index < list->count / 2)
			rval = DListItem_get_by_relative_index(list->begin, index, NULL);
		else
			rval = DListItem_get_by_relative_index(list->end,
					index - (list->count - 1), NULL);

		/* Check to see if we are removing everything to the end of the
		 * list.  If so, we need to modify the 'end' member. */
		if(index + count == list->count)
			list->end = rval->prev;

		rval = DListItem_pull_out_count(&rval, count);
	}

	list->count -= DListItem_count(rval);
	return(rval);
}

	/* Getters */
/* Returns the number of items in the list.
 *
 * Assumes 'list' is not null. */
size_t DList_get_count(DList* list){return(list->count);}

/* Returns the item at the given index.
 *
 * Returns:
 * 		NULL: Error, list is null or index is out of range.
 * 		ListItem*: The item found at the given index. */
ListItem* DList_get(DList* list, size_t index)
{
	if(!list || index >= list->count)return(NULL);

	if(!index)
		return((ListItem*)list->begin);
	else if(index < list->count / 2)
		return((ListItem*)DListItem_get_by_relative_index(list->begin,
				index, NULL));
	else
		return((ListItem*)DListItem_get_by_relative_index(list->end,
				index - (list->count - 1), NULL));
}
/* Returns the first item that has a value whose pointer points to the same
 * memory location as 'val'.
 *
 * Returns:
 * 		NULL: Error, no matching value.
 * 		DListItem*: A pointer to the item found. */
ListItem* DList_get_by_value(DList* list, void* val)
{
	return((ListItem*)DListItem_get_by_value(list->begin, val));
}
	/***********/

	/* Lifecycle */
/* Default constructor. */
DList* newDList()
{
	DList* list = malloc(sizeof(DList));
	if(!list)return(NULL);

	list->begin = NULL;
	list->end = NULL;
	list->count = 0;

	return(list);
}
/* Default destructor. */
void delDList(DList** list)
{
	if(!list || !*list)return;

	delDListItem_all(&(*list)->begin);

	free(*list);
	*list = NULL;
}
	/*************/
/******************************/
