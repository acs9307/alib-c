#ifndef LIST_ITEM_PRIVATE_IS_DEFINED
#define LIST_ITEM_PRIVATE_IS_DEFINED

#include "ListItem.h"
#include "ListItemVal_private.h"

/* Base class for all list items. */
struct ListItem
{
	/* We point to the value rather than inherit so that
	 * it can be in several lists at the same time. */
	ListItemVal* val;

	void* parent;

	/* If !0, then the item has been removed from the list and should be deleted whenever it is safe to do so. */
	char removed;
};

#endif
