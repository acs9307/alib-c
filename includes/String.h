#ifndef STRING_IS_DEFINED
#define STRING_IS_DEFINED

#include <string.h>

#include "alib_error.h"
#include "alib_types.h"
#include "alib_string.h"

/* Simple object inherited from BinaryBuffer.  Handles basic string operations
 * and ensures that there is always a null terminator at the end of the string.
 *
 * All buffer allocation is handled automatically.
 *
 * Note:
 * 		Though String inherits BinaryBuffer, it is not suggested that you be
 * 			very careful about using BinaryBuffer methods on this object as
 * 			it may corrupt the internal c-string because BinaryBuffer does
 * 			not worry about a null terminator.  */
typedef struct String String;

/*******Public Functions*******/
/* Clears the String.  This may deallocate the internal buffer if
 * the current buffer size is larger than the minimum size.
 * If you want to ensure the internal buffer is not freed, then use
 * 'String_clear_no_free()'. */
void String_clear(String* str);
/* Clears the String, but will not free any internal memory.
 * This gives a performance boost when the String is used
 * multiple times before being destroyed. */
void Stirng_clear_no_free(String* str);

/* Sets the value of the string, this will clear any old memory
 * and sets the value to 'str_val'.
 *
 * Parameters:
 * 		str: The object to modify.
 * 		str_val: A null terminated string to set as a value for 'str'.
 * 			The value is copied into 'str'. */
alib_error String_set(String* str, const char* str_val);
/* Same as 'String_set()' but only sets up to 'count' characters.
 *
 * If 'str_val' reaches a null terminator before 'count' is reached,
 * behavior is undefined.  This is by design.
 *
 * Parameters:
 * 		str: The object to modify.
 * 		str_val: A string to set as a value for 'str'.
 * 			The value is copied into 'str'.
 * 		count: The number of characters to copy over. */
alib_error String_set_count(String* str, const char* str_val, size_t count);

/* Appends a null-terminated string to the end of the String.
 *
 * Parameters:
 * 		str: The object to modify.
 * 		str_to_append: The null-terminated string to append to
 * 			the string.  Value will be copied into 'str'. */
alib_error String_append(String* str, const char* str_to_append);
/* Similar to 'String_append()' except a string of a certain length may be appended
 * that does not have to be null terminated.
 *
 * If 'str_val' reaches a null terminator before 'count' is reached,
 * behavior is undefined.  This is by design.
 *
 * Parameters:
 * 		str: The object to modify.
 * 		str_val: A string to set as a value for 'str'.
 * 			The value is copied into 'str'.
 * 		count: The number of characters to copy. */
alib_error String_append_count(String* str, const char* str_to_append, size_t count);
/* Similar to 'String_append()', but takes a 'String' object instead of a character
 * array.
 *
 * Parameters:
 * 		to: The object to modify.
 * 		from: The object that should be appended to the end of 'to'.
 * 			Only 'from's internal buffer will be copied, the 'from'
 * 			object will not be modified in any way. */
alib_error String_append_string(String* to, const String* from);
/* Similar to 'String_append()', but appends a single character instead of a character
 * array.
 *
 * Parameters:
 * 		str: The object to modify.
 * 		c: The character to append. */
alib_error String_append_char(String* str, char c);
/* Similar to 'String_append()', but appends an integer instead of a character
 * array.
 *
 * Parameters:
 * 		str: The object to modify.
 * 		val: The integer to append to the end of the string. */
alib_error String_append_int(String* str, const long long val);
/* Similar to 'String_append()', but appends an unsigned integer instead of a character
 * array.
 *
 * Parameters:
 * 		str: The object to modify.
 * 		val: The unsigned integer to append. */
alib_error String_append_uint(String* str, const unsigned long long val);
/* Similar to 'String_append()', but appends a double instead of a character
 * array.
 *
 * Parameters:
 * 		str: The object to modify.
 * 		val: The double to append. */
alib_error String_append_double(String* str, const double val);
/* Similar to 'String_append()', but appends a boolean instead of a character
 * array.
 *
 * Parameters:
 * 		str: The object to modify.
 * 		val: The value to append, 0 for false, !0 for true.
 * 		upper: If !0, the value will be appended to the string
 * 			in all upper case characters, otherwise all lower case
 * 			characters will be appended. */
alib_error String_append_bool(String* str, size_t val, char upper);
/* Appends a file to the end of the string.
 *
 * Parameters:
 * 		str: The String object to modify.
 * 		file: The file to read into the string.
 * 			If 'file' reaches EOF, appending is stopped.
 * 			Appending will start at the current position of 'file'.
 * 			Upon successful reading, 'file' should have EOF raised, if
 * 			later reading is required, the user must ensure to modify the
 * 			file positioning before and after the call to append. */
alib_error String_append_file(String* str, FILE* file);

/* Inserts a c-string at a specific index.
 *
 * Parameters:
 * 		str: The object to modify.
 * 		val: A null-terminated string to insert.  Value is copied.
 * 		index: The index to insert at. This will be the index of the first character
 * 			of 'val'. */
alib_error String_insert(String* str, const char* val, size_t index);
/* Same as 'String_insert()' except only a certain number of characters
 * will be copied.
 *
 * If 'val' contains a null terminator before 'count' is hit, behavior
 * is undefined.  This is by design.
 *
 * Parameters:
 * 		str: The object to modify.
 * 		val: The value to insert into 'str'.
 * 		index: The index to insert at. This will be the index of the first character
 * 			of 'val'.
 * 		count: The number of character to copy. */
alib_error String_insert_count(String* str, const char* val, size_t index,
		size_t count);
/* Same as 'String_insert()' except takes a 'String' object instead of a c-string.
 *
 * Parameters:
 * 		str: The object to modify.
 * 		val: The object to insert.
 * 		index: The index at which 'val' should be inserted. */
alib_error String_insert_string(String* str, const String* val, size_t index);
/* Same as 'String_insert()' except this inserts an integer.
 *
 * Parameters:
 * 		str: The object to modify.
 * 		val: The value to insert.
 * 		index: The index at which 'val' should be inserted. */
alib_error String_insert_int(String* str, const long long val,
		unsigned int index);
/* Same as 'String_insert()' except this inserts an unsigned integer.
 *
 * Parameters:
 * 		str: The object to modify.
 * 		val: The value to insert.
 * 		index: The index at which 'val' should be inserted. */
alib_error String_insert_uint(String* str, const unsigned long long val,
		unsigned int index);
/* Same as 'String_insert()' except this inserts a double.
 *
 * Parameters:
 * 		str: The object to modify.
 * 		val: The value to insert.
 * 		index: The index at which 'val' should be inserted. */
alib_error String_insert_double(String* str, const double val,
		unsigned int index);
/* Same as 'String_insert()' except this inserts a boolean.
 *
 * Parameters:
 * 		str: The object to modify.
 * 		val: The value to insert.
 * 		index: The index at which 'val' should be inserted. */
alib_error String_insert_bool(String* str, const unsigned char val,
		unsigned int index, char upper);

/* Copies one String to another String.
 *
 * Parameters:
 * 		to: The String to copy to.
 * 		from: The String to copy from. */
alib_error String_copy_string(String* to, const String* from);

/* Changes all the characters in 'str's buffer to lower case. */
void String_to_lower(String* str);
/* Changes all the characters in 'str's buffer to upper case. */
void String_to_upper(String* str);

/* Shrinks a string's capacity to fit the number of bytes
 * actually used. */
alib_error String_shrink_to_fit(String* str);

/* Removes a portion of a string from 'begin' to 'end'.
 *
 * Parameters:
 * 		str: The object to modify.
 * 		begin: The index to start removing from (inclusive).
 * 		end: The index to stop removing from (exclusive).  */
alib_error String_remove(String* str, size_t begin, size_t end);
/* Removes all matching substrings found in 'str'.
 *
 * Parameters:
 * 		str: The object to modify.
 * 		substring: The substrings to search for that are to be removed.
 * 			THIS MUST BE NULL TERMINATED, otherwise behavior is undefined.
 * 		substring_len: (OPTIONAL) The length of the substring. */
alib_error String_remove_substring(String* str, const char* substring);

/* Replaces a portion of the String with the given 'substring'.
 * This is basically the same as 'String_insert()' except that the
 * characters found at 'index' will be overwritten instead of pushed
 * back.
 *
 * Parameters:
 * 		str: The object to modify.
 * 		index: The index that should be replaced.
 * 		char_replace_count: The number of characters to replace.
 * 			If <0, the number of characters replaced will be the same as the
 * 			length of 'substring'.  If 0, then the function will work exactly
 * 			the same as 'String_insert()'.  Anything signifies the largest
 * 			number of characters to replace.  If the 'substring's null terminator
 * 			is hit before the count is reached, then only the number of bytes
 * 		substring: The substring to replace with.  */
alib_error String_replace(String* str, size_t index, long char_replace_count,
		const char* substring);
/* Same as 'String_replace()' but allows users to replace a portion of the string with a non null
 * terminated string.
 *
 * If 'substring' contains a null terminator before 'count' is hit, behavior
 * is undefined.  This is by design.
 *
 * Parameters:
 * 		str: The object to modify.
 * 		index: The index that should be replaced.
 * 		char_replace_count: The number of characters to replace.
 * 			If <0, the number of characters replaced will be the same as the
 * 			length of 'substring'.  If 0, then the function will work exactly
 * 			the same as 'String_insert()'.  Anything signifies the largest
 * 			number of characters to replace.  If the 'substring's null terminator
 * 			is hit before the count is reached, then only the number of bytes
 * 		substring: The substring to replace with.
 * 		count: The number of characters from 'substring' that should replace the characters
 * 			at 'index'. */
alib_error String_replace_count(String* str, size_t index, int char_replace_count,
		const char* substring, size_t count);

/* Searches for a substring in 'str' that matches 'substring' and replaces it with 'newstr'.
 *
 * Parameters:
 * 		str: The object to modify.
 * 		substring: The substring to replace.  MUST be null terminated.
 * 		newstr: The string to replace 'substring' with.
 * 		newstr_len: (OPTIONAL) The length of 'newstr'. */
alib_error String_replace_substrings(String* str, const char* substring,
		const char* newstr, size_t newstr_len);

/* Counts the number of matching substrings in the given String.
 *
 * Parameters:
 * 		str: The object search.
 * 		substring: The string to search for.
 *
 * Returns:
 * 		 <0: alib_error
 * 		0>=: Number of substrings found. */
long String_count_substrings(const String* str, const char* substring);

/* Returns a null terminated array of 'char*' pointers.
 *
 * Parameters:
 * 		str: The string to search.
 * 		substring: The c-string to search for.
 *
 * Returns:
 * 		NULL: Error.
 * 		char* const*: Success.  This points to a dynamically allocated
 * 			array of pointers which point to the internal buffer.  While
 * 			the returned value must be freed, the dereferenced values should
 * 			not be freed nor modified. */
char* const* String_get_substring_ptrs(const String* str, const char* substring);

/* Returns the internal buffer and clears the String.
 * The returned value is no longer handled by the String object and
 * therefore must be freed by the caller.
 *
 * Returns:
 * 		NULL: Either no internal memory is allocated by the String,
 * 			or an error occurred.
 * 		char*: The internal buffer of the String. */
char* String_extract_c_string(String* str);

	/* Getters */
/* Returns a pointer to the internal c-string.  The returned value should not be modified nor
 * freed.
 *
 * Assumes 'str' is not null. */
const char* String_get_c_string(String* str);
/* Returns the length of the String.
 *
 * Assumes 'str' is not null. */
size_t String_get_length(String* str);
/* Returns the number of bytes allocated by the internal buffer.
 *
 * Assumes 'str' is not null. */
size_t String_get_capacity(String* str);
/* Returns the index of the given pointer within the string.
 *
 * Returns:
 * 		0>=: Index of the pointer.
 * 		 <0: The given pointer does not belong to the String. */
long String_get_index(String* str, const char* ptr);
	/***********/
/******************************/

/*******Lifecycle*******/
	/* Constructors */
/* Default constructor. */
String* newString();
/* Copy constructor. */
String* newString_copy(String* str);
/* Creates a new String object and initializes it with the given
 * c-string.
 *
 * 'str' is copied.
 *
 * Parameters:
 * 		str: A character array to set as the content of the String.
 * 			If a null terminator is reached before 'len', behavior
 * 			is undefined.
 * 		len: The length of 'str'. */
String* newString_ex(const char* str, size_t len);
	/****************/

	/* Destructors */
/* Default destructor. */
void delString(String** str);
	/***************/
/***********************/

#endif
