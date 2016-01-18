#ifndef ALIB_STRING_IS_INCLUDED
#define ALIB_STRING_IS_INCLUDED

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

/*******Defines*******/
#if!defined ULLONG_MAX_CHAR_LEN
#define ULLONG_MAX_CHAR_LEN 20	/*This does not include a null terminator...*/
#endif
#if!defined LLONG_MAX_CHAR_LEN
#define LLONG_MAX_CHAR_LEN 20	/*This does not include a null terminator...*/
#endif
#if!defined DOUBLE_MAX_CHAR_LEN
#define DOUBLE_MAX_CHAR_LEN 32	/*Does not include null terminator...*/
#endif
/*********************/

/* Copies a string from 'from' to 'to' by iterating backwards.  Behaves
 * similarly to strcpy() except that copying is done backwards so that
 * overlapping strings where 'from' is before 'to' can be copied
 * safely.
 *
 * If the 'to' string is not large enough to hold all of the from string,
 * behavior is undefined.  If the 'to' and 'from' string are overlapping and
 * 'to' comes before 'from' in memory, behavior is undefined.
 *
 * Returns a pointer to the modified 'to' string.
 *
 * Assumes 'to' and 'from' are not null. */
char* strcpy_back(char* to, const char* from);
/* Copies a string from 'from' to 'to' by iterating backwards up to
 * a maximum number of characters specified by 'count'.  Behaves
 * similarly to strncpy() except that copying is done backwards so that
 * overlapping strings where 'from' is before 'to' can be copied
 * safely.  If a null terminator is hit before the 'count' is
 * reached, then only the calculated length of 'from' will be
 * copied over to 'to' instead of the entire count.
 *
 * If the 'to' string is not large enough to hold all of the from string,
 * behavior is undefined.  If the 'to' and 'from' string are overlapping and
 * 'to' comes before 'from' in memory, behavior is undefined.
 *
 * Returns a pointer to the modified 'to' string.
 *
 * Assumes 'to' and 'from' are not null. */
char* strncpy_back(char* to, const char* from, int count);

/* Safely copies a string from 'from' to 'to'.  Behaves similarly
 * to strncpy().  Basically will intelligently use strncpy() or strncpy_back()
 * so that overlapping strings can be copied without ill side effects.
 *
 * Parameters:
 * 		to: The string to modify.
 * 		from: The string to copy into the 'to' string.
 * 		count: The number of bytes to copy.
 *
 * Returns a pointer to the modified 'to' string.
 */
char* strncpy_safe(char* to, const char* from, size_t count);
/* Safely copies a string from 'from' to 'to'.  Behaves similarly
 * to strcpy().  Basically will intelligently use strcpy() or strcpy_back()
 * so that overlapping strings can be copied without ill side effects.
 *
 * Returns a pointer to the modified 'to' string.
 */
char* strcpy_safe(char* to, const char* from);

/* Works the same way as 'strncpy' except the 'to' value is set to a newly
 * allocated string.
 *
 * Parameters:
 * 		to: The point to be set to the new string. This
 * 			will be set to a dynamically allocated string which
 * 			MUST be freed by the caller. If set to NULL, then the
 * 			value will only be returned.
 * 		from: The string to copy to the 'to' pointer.
 * 		count: The number of bytes to copy, but will allocate
 * 			count + 1 bytes because we must put a null terminator in.
 *
 * Returns:
 * 		A dynamically allocated string that MUST be freed by the
 * 		caller.
 */
char* strncpy_alloc(char** to, const char* from, size_t count);
/* Works the same way as 'strcpy' except the 'to' value is set to a newly
 * allocated string.
 *
 * Parameters:
 * 		to: The point to be set to the new string. This
 * 			will be set to a dynamically allocated string which
 * 			MUST be freed by the caller.If set to NULL, then the
 * 			value will only be returned.
 * 		from: The string to copy to the 'to' pointer.
 *
 * Returns:
 * 		A dynamically allocated string that MUST be freed by the
 * 		caller.
 */
char* strcpy_alloc(char** to, const char* from);

/* Similar to 'strncpy()' except that it will only place a single null terminator at the end
 * of 'to' if a null terminator is hit before 'coppied_count' is reached.
 *
 * The reason this was created was so that strings could be quickly copied into large buffers
 * without wasting so much time setting the value of each byte in the buffer. */
char* strncpy_fast(char* to, const char* from, size_t count, size_t* coppied_count);

/* Same as memcpy() but copies backwards.  This is good if you need to copy memory
 * that overlaps where to 'to' is after the 'from'.
 *
 * Behavior is undefined if 'to' comes before 'from' when copying overlapping data.
 * If unsure which to use, use 'memcpy_safe()'.
 *
 * Parameters:
 * 		to: The memory block to modify.
 * 		from: The memory block to copy data from.
 * 		count: The number of bytes to copy.
 *
 * Returns 'to'.*/
void* memcpy_back(void* to, const void* from, size_t count);
/* Copies memory from one location to another.  This is done by choosing
 * either 'memcpy_back()' and 'memcpy()' to copy the memory so that overlapping
 * memory can be copied without any surprises.
 *
 * Parameters:
 * 		to: The memory block to modify.
 * 		from: The memory block to copy data from.
 * 		count: The number of bytes to copy.
 *
 * Returns 'to'. */
void* memcpy_safe(void* to, const void* from, size_t count);

/* Creates a new character string representation of the given integer
 * value.  The returned string is dynamically allocated and must be freed
 * by the caller. */
char* itoa(long long num);
/* Creates a new character string representation of the given unsigned integer
 * value.  The returned string is dynamically allocated and must be freed
 * by the caller. */
char* uitoa(unsigned long long num);
/* Creates a new character string representation of the given double
 * value.  The returned string is dynamically allocated and must be freed
 * by the caller. */
char* ftoa(double num);

/* Finds the next whitespace character in the string. A white space
 * is considered a space, tab, carriage return, or a new line character.
 *
 * If a null terminator is hit before a white space it found, NULL is
 * returned.
 */
char* find_next_whitespace(const char* str);
/* Finds the next non-whitespace character in the string. A white space
 * is considered a space, tab, carriage return, or a new line character.
 *
 * If a null terminator is hit before a white space it found, NULL is
 * returned.
 */
char* find_next_non_whitespace(const char* str);

/* Returns a substring of a string based on the two pointers.
 *
 * The returned value is a dynamically allocated string and must be
 * freed by the caller.
 *
 * Parameters:
 * 		begin_ptr: (Inclusive) Pointer to the beginning of the substring.
 * 		end_ptr: (Exclusive) Pointer to the end of the substring. */
char* make_substr(const char* begin_ptr, const char* end_ptr);

/* Finds the last character that matches the given character and returns
 * a pointer to the character within the given string.
 *
 * Parameters:
 *      str: The string to search in.
 *      c: The character to search for.
 *      count: The length of the string.
 *
 * Returns:
 *      NULL: No matching character was found.
 *      char*: A pointer to the last character in the string
 *          that matches 'c'.
 */
char* find_last_char_count(const char* str, char c, size_t count);
/* Finds the last character that matches the given character and returns
 * a pointer to the character within the given string.
 *
 * Parameters:
 *      str: The string to search in.  Must be null terminated.
 *      c: The character to search for.
 *
 * Returns:
 *      NULL: No matching character was found.
 *      char*: A pointer to the last character in the string
 *          that matches 'c'.
 */
char* find_last_char(const char* str, char c);

/* Compares strings, byte for byte, and ensures that
 * all the bytes match. This differs from strncmp as it will not
 * stop at a null terminator.
 *
 * Parameters:
 * 		str1: (REQUIRED)The first string for comparison
 * 		str1_len: (OPTIONAL)The length of the first string in bytes.
 * 			If 0, the value will be calculated with strlen().
 * 		str2: (REQUIRED) The second string for comparison.
 * 		str2_len: (OPTIONAL) The length of the second string
 * 			in bytes.
 *
 * Returns:
 * 		0: Strings match exactly.
 * 		>0: The first string's value is larger than the second.
 * 		<0: The second string's value is larger than the first. */
char str_match(const char* str1, size_t str1_len,
		const char* str2, size_t str2_len);

/* Converts all the characters in a null terminated string to their lower equivalent.
 *
 * If 'str' is not null terminated, behavior is undefined.
 *
 * Returns:
 * 		A pointer to 'str'. */
char* str_to_lower(char* str);
/* Converts all the characters, up to 'count', in a string to their lower equivalent.
 *
 * If 'count' exceeds the length of 'str', behavior is undefined.
 *
 * If a null terminator is hit, the function will stop converting immediately.
 *
 * Returns:
 * 		A pointer to 'str'. */
char* strn_to_lower(char* str, size_t count);
/* Converts all the characters in a null terminated string to their upper equivalent.
 *
 * If 'str' is not null terminated, behavior is undefined.
 *
 * Returns:
 * 		A pointer to 'str'. */
char* str_to_upper(char* str);
/* Converts all the characters, up to 'count', in a string to their upper equivalent.
 *
 * If 'count' exceeds the length of 'str', behavior is undefined.
 *
 * If a null terminator is hit, the function will stop converting immediately.
 *
 * Returns:
 * 		A pointer to 'str'. */
char* strn_to_upper(char* str, size_t count);
#endif
