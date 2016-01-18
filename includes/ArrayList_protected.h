#ifndef ARRAY_LIST_PROTECTED_IS_DEFINED
#define ARRAY_LIST_PROTECTED_IS_DEFINED

#include "ArrayList.h"

/*******PROTECTED FUNCTIONS*******/
/* Protected function used to remove items from the list pointer then
 * sets the pointer to NULL. This is useful when iterating through the list
 * removing items as you go because it does not search for the item, it assumes
 * a valid pointer is passed.  Will not call the delete callback on the item.
 *
 * If a non-list pointer, or a list pointer not connected to the passed list
 * is given, behavior is undefined.
 *
 * Parameters:
 * 		list: The list to remove the item from.
 * 		item: The item to remove from the list. */
void ArrayList_remove_by_ptr_no_free(ArrayList* list, void** item);
/* Protected function used to remove items from the list pointer then
 * sets the pointer to NULL. This is useful when iterating through the list
 * removing items as you go because it does not search for the item, it assumes
 * a valid pointer is passed.  Delete callback will be called on the item after
 * it is removed from the list.
 *
 * If a non-list pointer, or a list pointer not connected to the passed list
 * is given, behavior is undefined.
 *
 * Parameters:
 * 		list: The list to remove the item from.
 * 		item: The item to remove from the list. */
void ArrayList_remove_by_ptr(ArrayList* list, void** item);
/*********************************/

/*******Thread Safe Functions*******/
	/* PROTECTED FUNCTIONS */
/* Same as ArrayList_remove_by_ptr() but with mutexing. */
void ArrayList_remove_by_ptr_tsafe(ArrayList* list, void** item);
/* Same as ArrayList_remove_by_ptr_no_free() but with mutexing. */
void ArrayList_remove_by_ptr_no_free_tsafe(ArrayList* list, void** item);
	/***********************/
/***********************************/

#endif
