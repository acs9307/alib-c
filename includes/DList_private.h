#ifndef DLIST_PRIVATE_IS_DEFINED
#define DLIST_PRIVATE_IS_DEFINED

#include "DList.h"

struct DList
{
	DListItem* begin;
	DListItem* end;

	size_t count;
	/* If !0, then there are items that should be deleted from the list. */
	char items_ready_for_removal;
};

#endif
