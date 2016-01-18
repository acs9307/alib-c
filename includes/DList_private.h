#ifndef DLIST_PRIVATE_IS_DEFINED
#define DLIST_PRIVATE_IS_DEFINED

#include "DList.h"

struct DList
{
	DListItem* begin;
	DListItem* end;

	size_t count;
};

#endif
