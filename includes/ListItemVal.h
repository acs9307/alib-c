#ifndef LIST_ITEM_VAL_IS_DEFINED
#define LIST_ITEM_VAL_IS_DEFINED

#include <stdio.h>
#include <stdlib.h>

#include "alib_error.h"
#include "alib_types.h"

/* A referenced based object for storing data within a ListItem.
 * Due to referencing, the value can be placed in multiple locations,
 * whether it be in the same list or in separate lists without fear
 * of the value being freed while it is still being used. */
typedef struct ListItemVal ListItemVal;

/* Increments the reference counter for the object and returns
 * a pointer to the referenced object.  If referencing failed,
 * possibly due to the reference counter being corrupted,
 * NULL will be returned. */
ListItemVal* ListItemVal_ref(ListItemVal* liv);

/* Getters */
/* Returns the value of the ListItemVal object. */
void* ListItemVal_get_value(ListItemVal* liv);
/***********/

/*******Lifecycle*******/
ListItemVal* newListItemVal(void* val, alib_free_value free_val_cb);

/* Decrements the ListItemVal's reference counter and sets the pointer
 * to NULL.  If the reference counter reaches zero, the object will be
 * freed.
 *
 * If, for some reason, you must ensure the object is freed instantly without
 * waiting for each reference to call delete, call 'delListItemVal_complete()'. */
void delListItemVal(ListItemVal** liv);
/* This will force the ListItemVal to be freed instantly and not wait
 * for the reference count to reach zero.  THIS SHOULD NOT BE USED unless
 * you are absolutely sure you won't try to use this object again from any
 * of your references.
 *
 * It is best to use 'delListItemVal()', instead, whenever you have the
 * opportunity. */
void delListItemVal_complete(ListItemVal** liv);
/***********************/

#endif
