#ifndef ALIB_CB_FUNCS_IS_DEFINED
#define ALIB_CB_FUNCS_IS_DEFINED

/*******Comparison Callbacks*******/
/* Compares two integer pointer types.
 * This function is an 'alib_compare_values' type. */
int compare_int_ptr(const void* v_int1, const void* v_int2);
/* Compares two integer double pointer types.
 * This function is of type 'alib_compare_objects'. */
int compare_int_as_obj(const void** v_int1, const void** v_int2);
/**********************************/

#endif
