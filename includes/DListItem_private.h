#ifndef DLIST_ITEM_PRIVATE_IS_DEFINED
#define DLIST_ITEM_PRIVATE_IS_DEFINED

#include "DListItem.h"
#include "ListItem_private.h"
#include "ListItemVal_private.h"

/*******Class Declarations*******/
/* A doubly-linked list object.  It is named DListItem as it technically is just
 * a single item in the list, but a pointer to this object can, in fact, be
 * treated as a list.
 *
 * This is a minimalist object and therefore does not keep track of where the
 * beginning or end of the list is, nor does it keep track of how many items
 * are in the list, though it is capable of calculating all this value.
 *
 * Inherits from ListItem. */
struct DListItem
{
	ListItem base;

	struct DListItem* next;
	struct DListItem* prev;
};
/********************************/

#endif
