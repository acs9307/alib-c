#ifndef StringObject_is_included
#define StringObject_is_included

#include "alib_error.h"
#include "alib_string.h"

/* To increase performance and decrease memory usage, define
 * STRING_OBJECT_MIN_MEMORY to 1.  This will make the object
 * bare-bones therefore decreasing its memory footprint.  One way
 * this is accomplished is by removing all function pointers from
 * the object, therefore usage convenience will be decreased.
 *
 * Warning:
 * 	Using minimum memory may break code that uses the default
 * 		setting, however it is guaranteed that no alib library
 * 		function will be broken from turning on this setting. */

/*Defines*/
#ifndef STRING_OBJECT_MIN_MEMORY
#define STRING_OBJECT_MIN_MEMORY 0
#endif
/********/

/*******RETURN CODES*******/
/* Added only for backwards compatibility. */
enum
{
	/* Function was unable to allocate memory. */
	STRING_OBJECT_MEM_ERR = ALIB_MEM_ERR,
	/* Function was passed an invalid argument. */
	STRING_OBJECT_BAD_ARG = ALIB_BAD_ARG,
	/* Function completed successfully. */
	STRING_OBJECT_OK = ALIB_OK
};
/**************************/

/* ---DEPRECIATED---
 *
 * A string object which will manage allocation and deallocation of
 * memory as needed.  For speed, allocation will not always be exactly
 * the same size as actual data, this is to save allocation and
 * deallocation calls.  To check the actual memory usage of the string,
 * call 'capacity'.  The char pointer for the string data will always
 * be pointing to at least one character, and the string will always
 * be null terminated.
 *
 * NOTE:
 * 		String object can be used just like a c-string pointer.  In
 * 		other words, if you have a StringObject* string, you can print
 * 		it by calling printf(*string).*/
typedef struct StringObject
{
	/*DataMembers*/
	/* The c-string pointer for the string.
	 *
	 * If the string is unused, it will be set to "",
	 * therefore it will always be allocating at least one byte
	 * for the null terminator.  This is for safety when trying
	 * to use strlen().*/
	char* str;

	/* Length of the number of characters in the string.
	 * Length does not include null terminator. */
	unsigned int length;
	/* The number of bytes currently allocated by the
	 * object.*/
	unsigned int capacity;
	/*************/

	/*Settings*/
	/* The minimum amount of memory a string will keep allocated.
	 * Default is 64 bytes. */
	unsigned int min_mem;
	/**********/

#if !STRING_OBJECT_MIN_MEMORY
	/*Functions*/
	/* Sets the value of the StringObject to the given c-string.
	 *
	 * Return Codes:
	 * 		STRING_OBJECT_MEM_ERR
	 * 		STRING_OBJECT_BAD_ARG
	 * 		STRING_OBJECT_OK
	 */
	char(*set)(struct StringObject*, const char*);
	/* Sets the value of the StringObject to the given c-string of size 'count'.
	 *
	 * Return Codes:
	 * 		STRING_OBJECT_MEM_ERR
	 * 		STRING_OBJECT_BAD_ARG
	 * 		STRING_OBJECT_OK
	 */
	char (*set_count)(struct StringObject*, const char*, unsigned int);

	/* Clears the memory that has been allocated.  If the memory is
	 * larger than the minimum size, then the memory will be freed.
	 * If freeing memory is not desired, use StringObject_clear_no_free() instead.
	 */
	void(*clear)(struct StringObject*);

	/* Appends a null terminated c-string into the StringObject.
	 * If 'str_to_append' is not null terminated, results are undefined.
	 *
	 * Return Codes:
	 * 		STRING_OBJECT_MEM_ERR
	 * 		STRING_OBJECT_BAD_ARG
	 * 		STRING_OBJECT_OK
	 */
	char(*append)(struct StringObject*, const char*);
	/* Copies a c-string onto the end of a StringObject.  It will copy up to and not exceeding
	 * the size of the 'count' arg.  If the 'count' arg is zero, function will return STRING_OBJECT_OK.
	 *
	 * Return Codes:
	 * 		STRING_OBJECT_MEM_ERR
	 * 		STRING_OBJECT_BAD_ARG
	 * 		STRING_OBJECT_OK
	 */
	char(*append_count)(struct StringObject*, const char*, unsigned int count);
	/* Parses an integer into a string then appends it to the StringObject.
	 *
	 * Return Codes:
	 * 		STRING_OBJECT_MEM_ERR
	 * 		STRING_OBJECT_BAD_ARG
	 * 		STRING_OBJECT_OK
	 */
	char(*append_int)(struct StringObject*, const long long);
	/* Parses an unsigned integer into a string then appends it to the StringObject.
	 *
	 * Return Codes:
	 * 		STRING_OBJECT_MEM_ERR
	 * 		STRING_OBJECT_BAD_ARG
	 * 		STRING_OBJECT_OK
	 */
	char(*append_uint)(struct StringObject*, const unsigned long long);
	/* Parses a double into a string then appends it to the StringObject.
	 *
	 * Return Codes:
	 * 		STRING_OBJECT_MEM_ERR
	 * 		STRING_OBJECT_BAD_ARG
	 * 		STRING_OBJECT_OK
	 */
	char(*append_double)(struct StringObject*, const double);
	/* Parses an boolean into a string then appends it to the StringObject.
	 *
	 * Return Codes:
	 * 		STRING_OBJECT_MEM_ERR
	 * 		STRING_OBJECT_BAD_ARG
	 * 		STRING_OBJECT_OK
	 */
	char(*append_bool)(struct StringObject*, const unsigned char);

	/* Inserts a string into the string object by placing the first character
	 * of the 'val' string at the given index.  Everthing from the 'index' to the
	 * end of the original string will be moved down.
	 *
	 * Parameters:
	 * 		str: String object to insert into.
	 * 		val: The null terminated string to insert.
	 * 		index: The index at which to insert 'val'.
	 *
	 * Returns:
	 * 		StringObject error code.
	 */
	char(*insert)(struct StringObject*, const char*, unsigned int);
	char(*insert_count)(struct StringObject*, const char*, unsigned int,
			unsigned int);

	/* Replaces all characters in the given StringObject with lower-case
	 * equivalents.  If modifying the given string is not desired, please
	 * use StringObject_to_lower_copy(StringObject*).*/
	void (*to_lower)(struct StringObject*);
	/* Replaces all characters in the given StringObject with upper-case
	 * equivalents.  If modifying the given string is not desired, please
	 * use StringObject_to_upper_copy(StringObject*).*/
	void (*to_upper)(struct StringObject*);

	/* Shrinks the c-string to fit the size of its contents.
	 *
	 * Return Codes:
	 * 		STRING_OBJECT_MEM_ERR
	 * 		STRING_OBJECT_BAD_ARG
	 * 		STRING_OBJECT_OK
	 */
	char(*shrink_to_fit)(struct StringObject*);
	/***********/
#endif
} StringObject;

/*******Object Functions*******/
/* Sets the value of the StringObject to the given c-string.
 *
 * Return Codes:
 * 		STRING_OBJECT_MEM_ERR
 * 		STRING_OBJECT_BAD_ARG
 * 		STRING_OBJECT_OK
 */
char StringObject_set(StringObject* str, const char* str_value);
/* Sets the value of the StringObject to the given c-string of size 'count'.
 *
 * Return Codes:
 * 		STRING_OBJECT_MEM_ERR
 * 		STRING_OBJECT_BAD_ARG
 * 		STRING_OBJECT_OK
 */
char StringObject_set_count(StringObject* str, const char* str_value, unsigned int count);

/* Clears the memory that has been allocated.  If the memory is
 * larger than the minimum size, then the memory will be freed.
 * If freeing memory is not desired, use StringObject_clear_no_free() instead.
 */
void StringObject_clear(StringObject* str);
/* Clears the string, by setting the first character in the
 * string to null, but does not free any memory.
 */
void StringObject_clear_no_free(StringObject* str);

/* Appends a null terminated c-string into the StringObject.
 * If 'str_to_append' is not null terminated, results are undefined.
 *
 * Return Codes:
 * 		STRING_OBJECT_MEM_ERR
 * 		STRING_OBJECT_BAD_ARG
 * 		STRING_OBJECT_OK
 */
char StringObject_append(StringObject* str, const char* str_to_append);
/* Copies a c-string onto the end of a StringObject.  It will copy up to and not exceeding
 * the size of the 'count' arg.  If the 'count' arg is zero, function will return STRING_OBJECT_OK.
 *
 * Return Codes:
 * 		STRING_OBJECT_MEM_ERR
 * 		STRING_OBJECT_BAD_ARG
 * 		STRING_OBJECT_OK
 */
char StringObject_append_count(StringObject* str, const char* str_to_append, unsigned int count);
/* Appends a single character to the end of the string.
 *
 * Return Codes:
 * 		STRING_OBJECT_MEM_ERR
 * 		STRING_OBJECT_BAD_ARG
 * 		STRING_OBJECT_OK
 */
char StringObject_append_char(StringObject* str, char c);
/* Parses an integer into a string then appends it to the StringObject.
 *
 * Return Codes:
 * 		STRING_OBJECT_MEM_ERR
 * 		STRING_OBJECT_BAD_ARG
 * 		STRING_OBJECT_OK
 */
char StringObject_append_int(StringObject* str, const long long val);
/* Parses an unsigned integer into a string then appends it to the StringObject.
 *
 * Return Codes:
 * 		STRING_OBJECT_MEM_ERR
 * 		STRING_OBJECT_BAD_ARG
 * 		STRING_OBJECT_OK
 */
char StringObject_append_uint(StringObject* str, const unsigned long long val);
/* Parses a double into a string then appends it to the StringObject.
 *
 * Return Codes:
 * 		STRING_OBJECT_MEM_ERR
 * 		STRING_OBJECT_BAD_ARG
 * 		STRING_OBJECT_OK
 */
char StringObject_append_double(StringObject* str, const double val);
/* Parses an boolean into a string then appends it to the StringObject.
 *
 * Return Codes:
 * 		STRING_OBJECT_MEM_ERR
 * 		STRING_OBJECT_BAD_ARG
 * 		STRING_OBJECT_OK
 */
char StringObject_append_bool(StringObject* str, const unsigned char val);

/* Inserts a string into the string object by placing the first character
 * of the 'val' string at the given index.  Everything from the 'index' to the
 * end of the original string will be moved down.
 *
 * Parameters:
 * 		str: String object to insert into.
 * 		val: The null terminated string to insert.
 * 		index: The index at which to insert 'val'.
 *
 * Returns:
 * 		StringObject error code.
 */
char StringObject_insert(StringObject* str, const char* val, unsigned int index);
/* Inserts a string into the string object by placing the first character
 * of the 'val' string at the given index.  Everything from the 'index' to the
 * end of the original string will be moved down.  Will only insert up to
 * the number of characters specified or the first null terminator, which ever
 * comes first.
 *
 * Parameters:
 * 		str: String object to insert into.
 * 		val: The string to insert.
 * 		index: The index at which to insert 'val'.
 * 		val_len: The length of the string to insert.
 *
 * Returns:
 * 		StringObject error code.
 */
char StringObject_insert_count(StringObject* str, const char* val, unsigned int index,
		unsigned int val_len);
/* Inserts a signed integer into the string object by placing the first character
 * of the 'val' string at the given index.  Everything from the 'index' to the
 * end of the original string will be moved down.
 *
 * Parameters:
 * 		str: String object to insert into.
 * 		val: The signed integer to insert into the string.
 * 		index: The index at which to insert 'val'.
 *
 * Returns:
 * 		StringObject error code.
 */
char StringObject_insert_int(StringObject* str, const long long val,
		unsigned int index);
/* Inserts an unsigned integer into the string object by placing the first character
 * of the 'val' string at the given index.  Everything from the 'index' to the
 * end of the original string will be moved down.
 *
 * Parameters:
 * 		str: String object to insert into.
 * 		val: The unsigned integer to insert into the string.
 * 		index: The index at which to insert 'val'.
 *
 * Returns:
 * 		StringObject error code.
 */
char StringObject_insert_uint(StringObject* str, const unsigned long long val,
		unsigned int index);
/* Inserts a double into the string object by placing the first character
 * of the 'val' string at the given index.  Everything from the 'index' to the
 * end of the original string will be moved down.
 *
 * Parameters:
 * 		str: String object to insert into.
 * 		val: The double to insert into the string.
 * 		index: The index at which to insert 'val'.
 *
 * Returns:
 * 		StringObject error code.
 */
char StringObject_insert_double(StringObject* str, const double val,
		unsigned int index);
/* Inserts a boolean into the string object by placing the first character
 * of the 'val' string at the given index.  Everything from the 'index' to the
 * end of the original string will be moved down.  The boolean will be represented
 * as 'true' or 'false' in the string.
 *
 * Parameters:
 * 		str: String object to insert into.
 * 		val: The boolean to insert into the string.
 * 		index: The index at which to insert 'val'.
 *
 * Returns:
 * 		StringObject error code.
 */
char StringObject_insert_bool(StringObject* str, const unsigned char val,
		unsigned int index);

/* Replaces all characters in the given StringObject with lower-case
 * equivalents.  If modifying the given string is not desired, please
 * use StringObject_to_lower_copy(StringObject*).*/
void StringObject_to_lower(StringObject* str);
/* Does the same thing as StringObject_to_lower(StringObject*) except
 * that the given StringObject is not modified and it returns a
 * dynamically allocated character string of the lowered string.
 *
 * #REMEMBER to free the returned c-string.
 */
char* StringObject_to_lower_copy(StringObject* str);
/* Replaces all characters in the given StringObject with upper-case
 * equivalents.  If modifying the given string is not desired, please
 * use StringObject_to_upper_copy(StringObject*).*/
void StringObject_to_upper(StringObject* str);
/* Does the same thing as StringObject_to_upper(StringObject*) except
 * that the given StringObject is not modified and it returns a
 * dynamically allocated character string of the uppered string.
 *
 * #REMEMBER to free the returned c-string.
 */
char* StringObject_to_upper_copy(StringObject* str);

/* Shrinks the c-string to fit the size of its contents.
 *
 * Return Codes:
 * 		STRING_OBJECT_MEM_ERR
 * 		STRING_OBJECT_BAD_ARG
 * 		STRING_OBJECT_OK
 */
char StringObject_shrink_to_fit(StringObject* str);

/* Replaces all the substrings that matches 'substring' with 'newstr'.
 * If 'newstr' is null, then the 'substring's will be removed. */
StringObject* StringObject_replace_substrings(StringObject* str, const char* substring,
		const char* newstr);

/* Returns a pointer to the StringObject's c-string and then
 * resizes the string object to 0.  The original c-string will
 * not be deallocated by the StringObject and therefore is the
 * responsibility of the user.
 *
 * Return Values:
 * 		Non-null pointer on success.
 * 		Null pointer on error.  If an error occurs, str->str will
 * 			not be changed.
 */
char* StringObject_extract_c_string(StringObject* str);
/******************************/

/*Constructors*/
/* Creates a new StringObject with default values.
 * The c-string will be set to null and will not be allocated
 * until a function is called.
 *
 * Returns null if memory could not be allocated.
 */
StringObject* newStringObject();

/* Deallocates all memory used by the string object and sets the pointer to null. */
void delStringObject(StringObject** str);
/**************/

#endif
