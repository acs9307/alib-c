#ifndef LIST_ITEM_IS_DEFINED
#define LIST_ITEM_IS_DEFINED

#include "ListItemVal.h"

typedef struct ListItem ListItem;

/* Gets the value stored in the ListItem. */
void* ListItem_get_val(ListItem* item);
/* Gets a pointer to the ListItem's ListItemVal object member
 * and increments its reference counter. */
ListItemVal* ListItem_get_val_ref(ListItem* item);
/* Returns !0 if the item has been removed from the list, or 0 if it should remain. */
char ListItem_is_removed(ListItem* item);

/* Marks the item for removal. */
void ListItem_mark_for_removal(ListItem* item);

/*******Lifecycle*******/
#ifndef initListItem
/* Initializes a ListItem non-dynamic object.
 *
 * Parameters:
 * 		item (ListItem*): The item to initialize.
 * 		item_val (ListItemVal*): The value to set for the
 * 			item. */
#define initListItem(item, item_val)\
	do{\
	(item)->val = item_val;\
	(item)->parent = NULL;\
	(item)->removed = 0;\
	}while(0)
#endif

/* Constructs a new ListItem with the value set to the given
 * ListItemVal.
 *
 * NOTE:
 * 	This assumes that 'val' has been properly referenced, therefore
 * 		it will not increment the reference counter for 'val'.  If it
 * 		should be incremented, be sure to do so before calling this function.
 * 		If you must reuse a value, then call
 * 		'newListItem_ex(ListItemVal_ref(val));'. */
ListItem* newListItem_ex(ListItemVal* val);
/* Constructs a new ListItem. */
ListItem* newListItem(void* val, alib_free_value free_val);
/* Deletes a ListItem and decrements the reference counter of the
 * ListItemVal. */
void delListItem(ListItem** item);
/***********************/

#endif
