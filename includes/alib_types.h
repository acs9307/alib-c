#ifndef ALIB_TYPES_IS_INCLUDED
#define ALIB_TYPES_IS_INCLUDED

#include <ctype.h>
#include <stdio.h>

/*******SIMPLE TYPES*******/
	/* Boolean */
#ifndef bool
#define bool unsigned char
#endif

#ifndef true
#define true 1
#endif
#ifndef false
#define false 0
#endif

#ifndef TRUE_STRING
#define TRUE_STRING "true"
#endif
#ifndef TRUE_STRING_UPPER
#define TRUE_STRING_UPPER "TRUE"
#endif
#ifndef TRUE_STRING_LEN
#define TRUE_STRING_LEN 4
#endif
#ifndef FALSE_STRING
#define FALSE_STRING "false"
#endif
#ifndef FALSE_STRING_UPPER
#define FALSE_STRING_UPPER "FALSE"
#endif
#ifndef FALSE_STRING_LEN
#define FALSE_STRING_LEN 5
#endif
	/***********/

#ifndef byte
#define byte unsigned char
#endif
/**************************/

/*******ENUMERATIONS*******/
typedef enum string_type
{
	STRING_TYPE_INT = 0,
	STRING_TYPE_DOUBLE = 1,
	STRING_TYPE_BOOL = 2,
	STRING_TYPE_STRING = 3
}string_type;
/**************************/

/*******FUNCTION TYPES*******/
/* Function used to free a block of memory.  Should not modify the pointer value. */
typedef void (*alib_free_value)(void*);
/* Function used to delete an object, it should set the pointer to NULL if
 * the object was successfully destroyed. */
typedef void (*alib_del_obj)(void**);

/* Function used to allocate memory. */
typedef void* (*alib_alloc)();

/* A comparison function for two unknown types.
 *
 * Must return >0 if the first parameter is greater than the
 * second, 0 if they are equal, or <0 if the second is greater
 * than the first.
 *
 * The values of the parameters MUST not be modified. */
typedef int (*alib_compare_values)(const void*, const void*);
/* A comparison function for two unknown objects.  Exactly
 * the same as alib_compare_values, but is used for clarity so
 * that objects can implement this function and users know that
 * the passed pointer will be a double pointer rather than a single
 * pointer.
 *
 * Must return >0 if the first parameter is greater than the
 * second, 0 if they are equal, or <0 if the second is greater
 * than the first.
 *
 * The values of the parameters MUST not be modified. */
typedef int (*alib_compare_objects)(const void**, const void**);

/* Type used in 'pthread_create' as the function pointer for the
 * thread. This was made to make casting shorter. */
typedef void*(*pthread_proc)(void*);
/****************************/

/*******Structs*******/
	/* Wrapper Package */
/* Struct used for wrapping data, usually to associate
 * client data with a server. */
typedef struct wrapper_package
{
	/* Pointer to the parent package, usually should point
	 * to the object that created this wrapper package. */
	void* parent;
	/* Pointer to the user data for the package. */
	void* data;
	/* Callback to free the data. */
	alib_free_value free_data;
}wrapper_package;

		/* Constructors */
/* Instantiates a new 'wrapper_package' struct.  This must be freed
 * by calling either 'free_wrapper_package()' or 'del_wrapper_package()'.
 *
 * Parameters:
 * 		parent: The parent object of the struct.
 * 		data: The data associated with the struct.
 * 		free_data_cb: The callback that will be called to free the
 * 			given data.  If no callback is given, the 'data' member
 * 			will not be freed internally. */
wrapper_package* new_wrapper_package(void* parent, void* data,
		alib_free_value free_data_cb);
		/****************/

		/* Destructors */
/* Frees a wrapper package. */
void free_wrapper_package(wrapper_package* package);
/* Deletes a wrapper package and sets the pointer to null. */
void del_wrapper_package(wrapper_package** package);
		/***************/

		/* Destructor Callbacks */
/* Callback function that is of type 'alib_free_value'.
 *
 * Calls 'free_wrapper_package()'. */
void free_wrapper_package_cb(void* r_package);
/* Callback function that is of type 'alib_free_value'.
 *
 * Calls 'del_wrapper_package()'. */
void del_wrapper_package_cb(void** r_package);
		/************************/
	/*******************/
/*********************/

/*******Functions*******/
/* Returns true if the character is a whitespace,
 * otherwise returns false. */
char is_whitespace(char c);

/* Returns true if the null-terminated string is a valid integer.
 *
 * White spaces at beginning are ignored.  Function stops
 * checking the string after the second set of white-spaces are hit,
 * or a null terminator is hit.
 */
char is_int(const char* str);
/* Returns true if the string is a valid integer.
 *
 * White spaces at beginning are ignored.  Function stops
 * checking the string after the second set of white-spaces are hit,
 * or a null terminator is hit.
 */
char is_int_n(const char* str, int len);

/* Returns true if the null-terminated string is a valid double.
 *
 * White spaces at beginning are ignored.  Function stops
 * checking the string after the second set of white-spaces are hit,
 * or a null terminator is hit.
 */
char is_double(const char* str);
/* Returns true if the string is a valid double up to the given
 * length.
 *
 * White spaces at beginning are ignored.  Function stops
 * checking the string after the second set of white-spaces are hit,
 * or a null terminator is hit.
 *
 * Parameters:
 * 		str: The string to check for parsability.
 * 		len: The length of the string in bytes.
 */
char is_double_n(const char* str, size_t len);

/* Returns true if the string can be parsed into a boolean.
 *
 * White spaces at beginning are ignored.  Function stops
 * checking the string after the second set of white-spaces are hit,
 * or a null terminator is hit.
 */
char is_bool_n(const char* str, size_t str_len);
/* Returns true if the null-terminated string can be parsed into a boolean.
 *
 * White spaces at beginning are ignored.  Function stops
 * checking the string after the second set of white-spaces are hit,
 * or a null terminator is hit.
 */
char is_bool(const char* str);

/* Returns the type that the null-terminated string
 * can be parsed into. */
string_type get_string_type(const char* str);
/* Returns the type that the string can be parsed into. */
string_type get_string_type_n(const char* str, int len);
/***********************/

#endif
