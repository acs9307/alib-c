#include "ListItem_private.h"

/* Gets the value stored in the ListItem. */
void* ListItem_get_val(ListItem* item)
{
	return(item->val->value);
}
/* Gets a pointer to the ListItem's ListItemVal object member
 * and increments its reference counter. */
ListItemVal* ListItem_get_val_ref(ListItem* item)
{
	return(ListItemVal_ref(item->val));
}
/* Returns !0 if the item has been removed from the list, or 0 if it should remain. */
char ListItem_is_removed(ListItem* item)
{
	return(item->removed);
}
/* Marks the item for removal. */
void ListItem_mark_for_removal(ListItem* item)
{
	item->removed = 1;
}

/*******Lifecycle*******/
/* Constructs a new ListItem with the value set to the given
 * ListItemVal.
 *
 * NOTE:
 * 	This assumes that 'val' has been properly referenced, therefore
 * 		it will not increment the reference counter for 'val'.  If it
 * 		should be incremented, be sure to do so before calling this function.
 * 		If you must reuse a value, then call
 * 		'newListItem_ex(ListItemVal_ref(val));'. */
ListItem* newListItem_ex(ListItemVal* val)
{
	if(!val)return(NULL);

	ListItem* item = (ListItem*)malloc(sizeof(ListItem));
	if(!item)return(item);

	initListItem(item, val);

	return(item);
}
/* Constructs a new ListItem. */
ListItem* newListItem(void* val, alib_free_value free_val)
{
	return(newListItem_ex(newListItemVal(val, free_val)));
}
/* Deletes a ListItem and decrements the reference counter of the
 * ListItemVal. */
void delListItem(ListItem** item)
{
	if(!item || !*item)return;

	delListItemVal(&(*item)->val);

	free(*item);
	*item = NULL;
}
/***********************/
