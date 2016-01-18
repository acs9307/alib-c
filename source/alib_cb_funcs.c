#include "alib_cb_funcs.h"

/*******Comparison Callbacks*******/
/* Compares two integer pointer types.
 * This function is an 'alib_compare_values' type. */
int compare_int_ptr(const void* v_int1, const void* v_int2)
{
	int* int1 = (int*)v_int1;
	int* int2 = (int*)v_int2;

	return(*int1 - *int2);
}
/* Compares two integer double pointer types.
 * This function is of type 'alib_compare_objects'. */
int compare_int_as_obj(const void** v_int1, const void** v_int2)
{
	return(compare_int_ptr(*v_int1, *v_int2));
}
/**********************************/
