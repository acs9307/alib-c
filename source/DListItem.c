#include "DListItem_private.h"

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
DListItem* DListItem_insert_after(DListItem* list, DListItem* new_item)
{
	if(!list)
		return(DListItem_get_first_item(new_item));
	if(!new_item || new_item->base.parent)return(list);

	/* Get the beginning and end of the new item. */
	DListItem* new_item_end = DListItem_get_last_item(new_item);
	new_item = DListItem_get_first_item(new_item);
	DListItem_set_list_parent(new_item, list->base.parent);

	/* Set the next and previous pointers. */
	new_item_end->next = list->next;
	if(new_item_end->next)
		new_item_end->next->prev = new_item_end;
	new_item->prev = list;
	list->next = new_item;

	return(new_item);
}
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
DListItem* DListItem_insert_before(DListItem* list, DListItem* new_item)
{
	if(!list)return(DListItem_get_first_item(new_item));
	if(!new_item || new_item->base.parent)return(list);


	DListItem* new_item_end = DListItem_get_last_item(new_item);
	new_item = DListItem_get_first_item(new_item);
	DListItem_set_list_parent(new_item, list->base.parent);

	new_item_end->next = list;
	new_item->prev = list->prev;
	if(new_item_end->next)
		new_item_end->next->prev = new_item_end;
	new_item->prev->next = new_item;

	return(new_item);
}

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
DListItem* DListItem_push_back(DListItem* list, DListItem* new_item)
{
	if(!list)return(DListItem_get_first_item(new_item));
	if(!new_item || new_item->base.parent)return(list);

	list = DListItem_get_last_item(list);
	new_item = DListItem_get_first_item(new_item);
	DListItem_set_list_parent(new_item, list->base.parent);

	list->next = new_item;
	new_item->prev = list;

	return(new_item);
}
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
DListItem* DListItem_push_forward(DListItem* list, DListItem* new_item)
{
	if(!list)return(DListItem_get_first_item(new_item));
	if(!new_item || new_item->base.parent)return(list);

	DListItem* new_item_end = DListItem_get_last_item(new_item);
	new_item = DListItem_get_first_item(new_item);
	DListItem_set_list_parent(new_item, list->base.parent);
	list = DListItem_get_first_item(list);

	list->prev = new_item_end;
	new_item_end->next = list;

	return(new_item);
}

/* Moves an item from the 'from_index' to the 'to_index'.
 *
 * If 'to_index' exceeds LONG_MAX, behavior is undefined.
 *
 * Returns:
 * 		Success: The index that 'from_index' was placed.
 * 		Error: An alib_error code describing the error. */
long DListItem_move(DListItem* list, size_t from_index, size_t to_index)
{
	if(!list)return(ALIB_BAD_ARG);
	if(from_index == to_index)return((long)from_index);

	DListItem* from, *to;
	long exceeded;
	if(from_index < to_index)
	{
		from = DListItem_get(list, from_index, (size_t*)&exceeded);
			if(exceeded)return(ALIB_BAD_INDEX);
		to = DListItem_get_by_relative_index(from,
				to_index - from_index, &exceeded);

		if(from != to)
		{
			/* Place 'from' after 'to'. */
				/* Remove from old location. */
			if(from->prev)
				from->prev->next = from->next;
			if(from->next)
				from->next->prev = from->prev;
				/* Place between 'to' and its next. */
			from->prev = to;
			from->next = to->next;
			to->next = from;
			if(from->next)
				from->next->prev = from;

			/* Check find where we placed 'to'. */
			to_index -= exceeded;
		}
		else
			to_index = from_index;
	}
	else
	{
		to = DListItem_get(list, to_index, (size_t*)&exceeded);
			if(exceeded)return(ALIB_BAD_INDEX);
		from = DListItem_get_by_relative_index(to, from_index - to_index,
				&exceeded);
			if(exceeded)return(ALIB_BAD_INDEX);

		if(from != to)
		{
			/* Place 'from' at 'to's location and set 'to' to
			 * be after 'from'. */
			/* Remove from old location. */
			if(from->prev)
				from->prev->next = from->next;
			if(from->next)
				from->next->prev = from->prev;
			/* Place before 'to'. */
			from->prev = to->prev;
			from->next = to;
			to->prev = from;
			if(from->prev)
				from->prev->next = from;
		}
		else
			to_index = from_index;
	}

	return((long)to_index);
}
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
		size_t to_index, size_t count)
{
	if(!list)return(ALIB_BAD_ARG);

	DListItem* from, *from_end, *to;
	long exceeded;
	if(from_index < to_index)
	{
		from = DListItem_get(list, from_index, (size_t*)&exceeded);
			/* Error.  Can't move if the item doesn't exist. */
			if(exceeded)return(ALIB_BAD_INDEX);
		from_end = DListItem_get_by_relative_index(from,
				count - 1, &exceeded);
			/* There is no way 'to' can be past 'from_end' so
			 * we can't move.  Everything good. */
			if(exceeded || !from_end->next)return(from_index);
		to = DListItem_get_by_relative_index(from_end,
				to_index - from_index, &exceeded);

		/* Place 'from' after 'to'. */
			/* Remove from old location. */
		if(from->prev)
			from->prev->next = from_end->next;
		if(from_end->next)
			from_end->next->prev = from->prev;
			/* Place between 'to' and its previous. */
		from->prev = to->prev;
		from_end->next = to;
		to->prev = from_end;
		if(from->prev)
			from->prev->next = from;

		if(exceeded)
			to_index -= exceeded;
	}
	else
	{
		to = DListItem_get(list, to_index, (size_t*)&exceeded);
			/* Error. Nothing to move. */
			if(exceeded)return(ALIB_BAD_INDEX);
		from = DListItem_get_by_relative_index(to, from_index - to_index,
				&exceeded);
			/* Error. Nothing to move. */
			if(exceeded)return(ALIB_BAD_INDEX);
		from_end = DListItem_get_by_relative_index(from, count - 1, NULL);


		/* Place 'from' before 'to'. */
			/* Remove from old location. */
		if(from->prev)
			from->prev->next = from_end->next;
		if(from_end->next)
			from_end->next->prev = from->prev;
			/* Place before 'to'. */
		from->prev = to->prev;
		from_end->next = to;
		to->prev = from_end;
		if(from->prev)
			from->prev->next = from;
	}

	return(to_index);
}

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
DListItem* DListItem_pull_out(DListItem** item)
{
	if(!item || !*item)return(NULL);

	/* Store the pulled item. */
	DListItem* pulled_item = *item;

	/* Set item to point to an adjacent item in the list. */
	if((*item)->prev)
		*item = (*item)->prev;
	else
		*item = (*item)->next;

	/* The pulled item no longer belongs to a list,
	 * modify the next and previous pointers to reflect this. */
	if(pulled_item->next)
		pulled_item->next->prev = pulled_item->prev;
	if(pulled_item->prev)
		pulled_item->prev->next = pulled_item->next;
	pulled_item->next = NULL;
	pulled_item->prev = NULL;
	DListItem_set_list_parent(pulled_item, NULL);
	return(pulled_item);
}
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
DListItem* DListItem_pull_out_count(DListItem** item, size_t count)
{
	if(!item || !*item || !count)return(NULL);

	DListItem* pulled_item_begin = *item;
	DListItem* pulled_item_end = DListItem_get_by_relative_index(*item,
			count - 1, NULL);

	/* Set item to point to an adjacent item in the list. */
	if((*item)->prev)
		*item = (*item)->prev;
	else
		*item = pulled_item_end->next;

	/* The pulled item no longer belongs to a list,
	 * modify the next and previous pointers to reflect this. */
	if(pulled_item_begin->prev)
		pulled_item_begin->prev->next = pulled_item_end->next;
	if(pulled_item_end->next)
		pulled_item_end->next->prev = pulled_item_begin->prev;
	pulled_item_begin->prev = NULL;
	pulled_item_end->next = NULL;
	DListItem_set_list_parent(pulled_item_begin, NULL);
	return(pulled_item_begin);
}

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
DListItem* DListItem_get(DListItem* list, size_t index, size_t* index_exceeds)
{
	if(!list)
		return(NULL);

	/* Go to the beginning of the list. */
	for(; list->prev; list = list->prev);

	/* Start going forwards through the list until the index is found. */
	for(; index > 0 && list->next; list = list->next, --index);

	/* See if the index exceeded the length of the list. */
	if(index_exceeds)
		*index_exceeds = index;

	return(list);
}
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
DListItem* DListItem_get_by_value(DListItem* list, void* value)
{
	DListItem* list_it;

	/* Check bad arguments.
	 * Value is not checked as we may have items whose values are null. */
	if(!list)return(NULL);

	/* Check the front of the list first. */
	list_it = list;
	for(; list_it && list_it->base.val->value != value; list_it = list_it->prev);
	if(list_it)
		return(list_it);

	/* If not found, check the end of the list. */
	for(list = list->next; list && list->base.val->value != value;
			list = list->next);
	return(list);
}
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
		long* index_exceeds)
{
	if(!list)return(NULL);

	if(index > 0)
		for(; index && list->next; --index, list = list->next);
	else
		for(; index && list->prev; ++index, list = list->prev);

	/* See if the index exceeded the length of the list. */
	if(index_exceeds)
		*index_exceeds = index;

	return(list);
}

/* Returns the number of items that are in the list based on a
 * single DListItem pointer.
 *
 * Parameters:
 * 		list: A DListItem pointer that belongs to the list whose
 * 			size should be calculated.
 *
 * Returns:
 * 		Number of DListItems found in the list. */
size_t DListItem_count(DListItem* list)
{
	/* Check for bad argument. */
	if(!list)return(0);

	size_t count = 0;
	DListItem* it;

	/* Check the first half first. */
	for(it = list->prev; it; it = it->prev, ++count);

	/* Move to the end of the list. */
	for(; list; list = list->next, ++count);

	return(count);
}
/* Gets the index of the given DListItem.
 *
 * Returns:
 * 		>=0 - Index of the item in the list.
 * 		<0  - Error code. */
int DListItem_index(DListItem* list)
{
	if(!list)return(ALIB_BAD_ARG);

	DListItem* begin = DListItem_get_first_item(list);
	int index = 0;

	for(; begin != list; begin = begin->next, ++index);

	if(!begin)
		return(ALIB_BAD_ARG);
	else
		return(index);
}

/* Returns the first item in the list. */
DListItem* DListItem_get_first_item(DListItem* list)
{
	if(!list)return(NULL);

	/* Iterate backwards through the list. */
	while(list->prev)list = list->prev;

	return(list);
}
/* Returns the last item in the list. */
DListItem* DListItem_get_last_item(DListItem* list)
{
	if(!list)return(NULL);

	/* Iterate forward though the list till the end. */
	while(list->next)list = list->next;

	return(list);
}

/* Returns the next item in the list.
 *
 * Assumes 'list' is not null. */
DListItem* DListItem_get_next_item(DListItem* list)
{
	return(list->next);
}
/* Returns the previous item in the list.
 *
 * Assumes 'list' is not null. */
DListItem* DListItem_get_prev_item(DListItem* list)
{
	return(list->prev);
}

/* Returns the list item's value.
 *
 * Assumes 'item' is not null. */
void* DListItem_get_value(DListItem* item)
{
	return(item->base.val->value);
}
/* Returns a pointer to the item's ListItemVal object.
 *
 * Assumes 'item' is not null. */
ListItemVal* DListItem_get_value_object(DListItem* item)
{
	return(item->base.val);
}
	/***********/

	/* Setters */
/* Sets the parent of the list for each item in the list. */
alib_error DListItem_set_list_parent(DListItem* list, void* parent)
{
	if(!list)return(ALIB_BAD_ARG);

	/* Nothing to do. */
	if(list->base.parent == parent)return(ALIB_OK);

	/* Ensure that we get the first item in the list, we need to
	 * set the parent for each item. */
	list = DListItem_get_first_item(list);
	while(list)
	{
		list->base.parent = parent;
		list = list->next;
	}

	return(ALIB_OK);
}
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
		DListItem** old_list)
{
	DListItem* list = (DListItem*)malloc(sizeof(DListItem));
	if(!list)return(NULL);

	/* Initialize base. */
	initListItem((ListItem*)list, newListItemVal(item_value, free_value_cb));

	/* Set the next and previous pointers. */
	if(!old_list || !*old_list)
	{
		list->next = NULL;
		list->prev = NULL;
	}
	else
	{
		list->prev = (*old_list);
		list->next = (*old_list)->next;
		if(list->next)
			list->next->prev = list;
		list->prev->next = list;
	}

	/* Set the old list, if possible. */
	if(old_list)
		*old_list = list;
	return(list);
}
/* Deletes the given list item and sets the pointer to an adjacent
 * DListItem if possible (preferably the previous item).  If not,
 * the pointer will be set to NULL. */
void delDListItem(DListItem** list_item)
{
	DListItem* del_item;

	if(!list_item || !*list_item)return;

	/* Set the list_item pointer to an adjacent
	 * DListItem if possible.  If not, set to NULL. */
	del_item = (*list_item);
	if((*list_item)->prev)
		(*list_item) = (*list_item)->prev;
	else
		*list_item = (*list_item)->next;

	/* Remove the item from the list as needed and
	 * then free the item. */
	if(del_item->prev)
		del_item->prev->next = del_item->next;
	if(del_item->next)
		del_item->next->prev = del_item->prev;

	/* This will handle freeing of value members as well
	 * as the object pointer. */
	delListItem((ListItem**)&del_item);
}

/* Calls delDListItem on all the DListItem objects in the list. */
void delDListItem_all(DListItem** list_item)
{
	while(*list_item)
		delDListItem(list_item);
}
/**************************/
