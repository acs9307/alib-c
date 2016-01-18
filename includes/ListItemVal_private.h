#ifndef LIST_ITEM_VAL_PRIVATE_IS_DEFINED
#define LIST_ITEM_VAL_PRIVATE_IS_DEFINED

#include "ListItemVal.h"

/* A referenced based object for storing data within a ListItem.
 * Due to referencing, the value can be placed in multiple locations,
 * whether it be in the same list or in separate lists without fear
 * of the value being freed while it is still being used. */
struct ListItemVal
{
	void* value;
	alib_free_value free_value_cb;

	size_t ref_count;
};

#endif
