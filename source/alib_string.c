#include "alib_string.h"

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
char* strcpy_back(char* to, const char* from)
{
	const char* start_ptr = from - 1;
	char to_nt_hit = 0;

	/* Find the end of from string. */
	for(; *from != '\0'; ++from, ++to)
		if(*to == '\0')
			to_nt_hit = 1;

	/* Ensure 'to' is null terminated. */
	if(*to == '\0' || to_nt_hit)
		*(to + 1) = '\0';

	/* Start copying the string. */
	for(; from != start_ptr; --from, --to)
		*to = *from;

	return(to);
}
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
char* strncpy_back(char* to, const char* from, int count)
{
	const char* start_ptr = from - 1;
	char to_nt_hit = 0;

	/* Find end of strings. */
	for(; count > 0; ++from, ++to, --count)
	{
		if(*to == '\0')
			to_nt_hit = 1;
		if(*from == '\0')
			break;
	}

	/* Ensure 'to' is null terminated. */
	if(*to == '\0' || to_nt_hit)
		*(to + 1) = '\0';

	/* Copy values. */
	for(; from != start_ptr; --from, --to)
		*to = *from;

	return(to);
}

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
char* strncpy_safe(char* to, const char* from, size_t count)
{
	if(!to || !from || to == from)return(to);

	/* If the 'to' comes after the 'from', we must copy backwards. */
	if(to > from && to < from + count)
		return(strncpy_back(to, from, count));
	else
		return(strncpy(to, from, count));
}
/* Safely copies a string from 'from' to 'to'.  Behaves similarly
 * to strcpy().  Basically will intelligently use strcpy() or strcpy_back()
 * so that overlapping strings can be copied without ill side effects.
 *
 * Returns a pointer to the modified 'to' string.
 */
char* strcpy_safe(char* to, const char* from)
{
	if(!to || !from)
		return(to);

	/* If the 'to' comes after the 'from', we must copy backwards. */
	if(to > from)
		return(strcpy_back(to, from));
	else
		return(strcpy(to, from));
}

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
char* strncpy_alloc(char** to, const char* from, size_t count)
{
	char* new_str;

	/* Check for errors. */
	if(!from)
		return((*to)?*to:NULL);

	/* Allocate memory and copy data. */
	new_str = (char*)malloc(count + 1);
	strncpy(new_str, from, count);
	new_str[count] = '\0';

	/* Set pointer and return. */
	if(to)
		*to = new_str;
	return(new_str);
}
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
char* strcpy_alloc(char** to, const char* from)
{
	return(strncpy_alloc(to, from, strlen(from)));
}

/* Similar to 'strncpy()' except that it will only place a single null terminator at the end
 * of 'to' if a null terminator is hit before 'coppied_count' is reached.
 *
 * The reason this was created was so that strings could be quickly copied into large buffers
 * without wasting so much time setting the value of each byte in the buffer. */
char* strncpy_fast(char* to, const char* from, size_t count, size_t* coppied_count)
{
	char* to_begin = to;
	if(coppied_count)
		*coppied_count = count;

	for(; *from != '\0' && count; --count, ++from, ++to)
		*to = *from;
	if(count)
	{
		*to = *from;
		--count;
	}

	if(coppied_count)
		coppied_count -= count;

	return(to_begin);
}

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
void* memcpy_back(void* t, const void* f, size_t count)
{  
	if(!t || !f)return(t);

        const unsigned char* from = (const unsigned char*)f;
        unsigned char* to = (unsigned char*)t;

	/* Must modify the count to put it into index
	 * mode. */
	from += count - 1;
	to += count - 1;

	for(; count > 0; --count, --from, --to)
		*((char*)to) = *((char*)from);

	return((void*)to);
}
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
void* memcpy_safe(void* to, const void* from, size_t count)
{
	if(!to || !from)return(to);

	if(to > from)
		memcpy_back(to, from, count);
	else
		memcpy(to, from, count);

	return(to);
}

/* Creates a new character string representation of the given integer
 * value.  The returned string is dynamically allocated and must be freed
 * by the caller. */
char* itoa(long long num)
{
       char* str = malloc(LLONG_MAX_CHAR_LEN + 1);
       if(!str)return(NULL);

       sprintf(str, "%lld", num);
       str = realloc(str, strlen(str) + 1);
       return(str);
}
/* Creates a new character string representation of the given unsigned integer
 * value.  The returned string is dynamically allocated and must be freed
 * by the caller. */
char* uitoa(unsigned long long num)
{
	char* str = (char*)malloc(ULLONG_MAX_CHAR_LEN + 1);
	if(!str)return(NULL);

#if __linux__
	sprintf(str, "%llu", num);
#else
	sprintf(str, "%lu", num);
#endif
	str = (char*)realloc(str, strlen(str) + 1);
	return(str);
}
/* Creates a new character string representation of the given double
 * value.  The returned string is dynamically allocated and must be freed
 * by the caller. */
char* ftoa(double num)
{
	char* str = (char*)malloc(DOUBLE_MAX_CHAR_LEN + 1);
	if(!str)return(NULL);

	sprintf(str, "%f", num);
	str = (char*)realloc(str, strlen(str) + 1);
	return(str);
}

/* Finds the next whitespace character in the string. A white space
 * is considered a space, tab, carriage return, or a new line character.
 *
 * If a null terminator is hit before a white space it found, NULL is
 * returned.
 */
char* find_next_whitespace(const char* str)
{
	if(!str)return(NULL);

	for(;*str != ' ' && *str != '\t' && *str != '\n' && *str != '\r'
			&& *str != '\0'; ++str);
	if(*str == '\0')
		return(NULL);
	else
		return((char*)str);
}
/* Finds the next non-whitespace character in the string. A white space
 * is considered a space, tab, carriage return, or a new line character.
 *
 * If a null terminator is hit before a white space it found, NULL is
 * returned.
 */
char* find_next_non_whitespace(const char* str)
{
	if(!str)return(NULL);

	for(;*str == ' ' || *str == '\t' || *str == '\n' || *str == '\r' ||
		*str == '\0'; ++str);
	if(*str == '\0')
		return(NULL);
	else
		return((char*)str);
}

/* Returns a substring of a string based on the two pointers.
 *
 * The returned value is a dynamically allocated string and must be
 * freed by the caller.
 *
 * Parameters:
 * 		begin_ptr: (Inclusive) Pointer to the beginning of the substring.
 * 		end_ptr: (Exclusive) Pointer to the end of the substring. */
char* make_substr(const char* begin_ptr, const char* end_ptr)
{
	if(end_ptr < begin_ptr)return(NULL);
	return(strncpy_alloc(NULL, begin_ptr, end_ptr - begin_ptr));
}

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
char* find_last_char_count(const char* str, char c, size_t count)
{
    char* r_ptr;

    /* Check args. */
    if(!str)return(NULL);

    /* Set the pointer to the end of the string,
     * we will iterate backwards. */
    r_ptr = (char*)(str + count);

    /* Iterate backwards through the string. */
    for(; *r_ptr != c && r_ptr != str - 1; --r_ptr);
    if(*r_ptr == c)
        return(r_ptr);
    else
        return(NULL);
}
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
char* find_last_char(const char* str, char c)
{
    return(find_last_char_count(str, c, strlen(str)));
}

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
		const char* str2, size_t str2_len)
{
	/* Big iterators for comparing larger blocks of data at a time. */
	const size_t* cmp_big_it;
	const size_t* cmp_big_it2;
	const char* cmp_it;
	const char* cmp_it2;

	/* Ensure we don't have a null pointer. */
	if(!str1 || !str2)
	{
		if(str1 == str2)
			return(0);
		else if(str1)
			return(1);
		else
			return(-1);
	}

	/* Calculate the lengths, if necessary, and check if their
	 * lengths are the same. */
	if(!str1_len)
		str1_len = strlen(str1);
	if(!str2_len)
		str2_len = strlen(str2);
	if(str1_len < str2_len)
		return(-1);
	else if(str1_len > str2_len)
		return(1);

	/* Compare by iterating the the big iterators first. */
	for(cmp_big_it = (size_t*)str1, cmp_big_it2 = (size_t*)str2;
			*cmp_big_it == *cmp_big_it2 && str1_len > sizeof(size_t);
			str1_len -= sizeof(size_t), ++cmp_big_it, ++cmp_big_it2);
	if(str1_len > sizeof(size_t))
	{
		if(*cmp_big_it < *cmp_big_it2)
			return(-1);
		else
			return(1);
	}

	/* Now check the smaller section of the string. */
	for(cmp_it = (char*)cmp_big_it, cmp_it2 = (char*)cmp_big_it2;
			*cmp_it == *cmp_it2 && str1_len;
			--str1_len, ++cmp_it, ++cmp_it2);

	if(!str1_len)
		return(0);
	else if(*cmp_it < *cmp_it2)
		return(-1);
	else
		return(1);
}

/* Converts all the characters in a null terminated string to their lower equivalent.
 *
 * If 'str' is not null terminated, behavior is undefined.
 *
 * Returns:
 * 		A pointer to 'str'. */
char* str_to_lower(char* str)
{
	if(!str)return(str);

	char* str_it = str;
	for(; *str_it; ++str_it)
		*str_it = tolower(*str_it);

	return(str);
}
/* Converts all the characters, up to 'count', in a string to their lower equivalent.
 *
 * If 'count' exceeds the length of 'str', behavior is undefined.
 *
 * If a null terminator is hit, the function will stop converting immediately.
 *
 * Returns:
 * 		A pointer to 'str'. */
char* strn_to_lower(char* str, size_t count)
{
	if(!str)return(str);

	char* str_it = str;
	for(; *str_it && count; ++str_it, --count)
		*str_it = tolower(*str_it);

	return(str);
}
/* Converts all the characters in a null terminated string to their upper equivalent.
 *
 * If 'str' is not null terminated, behavior is undefined.
 *
 * Returns:
 * 		A pointer to 'str'. */
char* str_to_upper(char* str)
{
	if(!str)return(str);

	char* str_it = str;
	for(; *str_it; ++str_it)
		*str_it = toupper(*str_it);

	return(str);
}
/* Converts all the characters, up to 'count', in a string to their upper equivalent.
 *
 * If 'count' exceeds the length of 'str', behavior is undefined.
 *
 * If a null terminator is hit, the function will stop converting immediately.
 *
 * Returns:
 * 		A pointer to 'str'. */
char* strn_to_upper(char* str, size_t count)
{
	if(!str)return(str);

	char* str_it = str;
	for(; *str_it && count; ++str_it, --count)
		*str_it = toupper(*str_it);

	return(str);
}
