#include "String_private.h"

/*******Public Functions*******/
/* Clears the String.  This may deallocate the internal buffer if
 * the current buffer size is larger than the minimum size.
 * If you want to ensure the internal buffer is not freed, then use
 * 'String_clear_no_free()'. */
void String_clear(String* str)
{
	if(str)
	{
		BinaryBuffer_clear(&str->base);

		/* If we still have memory allocated, then
		 * we simply put a null terminator at the beginning of
		 * the buffer. */
		if(str->base.buff)
		{
			*str->base.buff = 0;
			str->base.len = 1;
		}
	}
}
/* Clears the String, but will not free any internal memory.
 * This gives a performance boost when the String is used
 * multiple times before being destroyed. */
void Stirng_clear_no_free(String* str)
{
	if(str)
	{
		BinaryBuffer_clear_no_resize(&str->base);
		if(str->base.buff)
		{
			*str->base.buff = 0;
			str->base.len = 1;
		}
	}
}

/* Sets the value of the string, this will clear any old memory
 * and sets the value to 'str_val'.
 *
 * Parameters:
 * 		str: The object to modify.
 * 		str_val: A null terminated string to set as a value for 'str'.
 * 			The value is copied into 'str'. */
alib_error String_set(String* str, const char* str_val)
{
	if(!str_val)
	{
		String_clear(str);
		return(ALIB_OK);
	}
	else
		return(String_set_count(str, str_val, strlen(str_val)));
}
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
alib_error String_set_count(String* str, const char* str_val, size_t count)
{
	if(!str)return(ALIB_BAD_ARG);

	BinaryBuffer_clear(&str->base);
	BinaryBuffer_append(&str->base, str_val, count);
	BinaryBuffer_append(&str->base, "\0", 1);

	return(ALIB_OK);
}

/* Appends a null-terminated string to the end of the String.
 *
 * Parameters:
 * 		str: The object to modify.
 * 		str_to_append: The null-terminated string to append to
 * 			the string.  Value will be copied into 'str'. */
alib_error String_append(String* str, const char* str_to_append)
{
	return(String_append_count(str, str_to_append, strlen(str_to_append)));
}
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
alib_error String_append_count(String* str, const char* str_to_append, size_t count)
{
	alib_error err;

	if(!str || !str_to_append)return(ALIB_BAD_ARG);

	if(str->base.len > 1)
		err = BinaryBuffer_insert(&str->base, str->base.len - 1, str_to_append, count);
	else
	{
		err = BinaryBuffer_append(&str->base, str_to_append, count);
			if(err)return(err);
		err = BinaryBuffer_append(&str->base, "\0", 1);
	}

	return(err);
}
/* Similar to 'String_append()', but takes a 'String' object instead of a character
 * array.
 *
 * Parameters:
 * 		to: The object to modify.
 * 		from: The object that should be appended to the end of 'to'.
 * 			Only 'from's internal buffer will be copied, the 'from'
 * 			object will not be modified in any way. */
alib_error String_append_string(String* to, const String* from)
{
	if(!from)return(ALIB_BAD_ARG);

	return(String_append(to, (char*)from->base.buff));
}
/* Similar to 'String_append()', but appends a single character instead of a character
 * array.
 *
 * Parameters:
 * 		str: The object to modify.
 * 		c: The character to append. */
alib_error String_append_char(String* str, char c)
{
	return(String_append_count(str, &c, 1));
}
/* Similar to 'String_append()', but appends an integer instead of a character
 * array.
 *
 * Parameters:
 * 		str: The object to modify.
 * 		val: The integer to append to the end of the string. */
alib_error String_append_int(String* str, const long long val)
{
	char new_str[LLONG_MAX_CHAR_LEN + 1];
	sprintf(new_str, "%lld", val);
	return(String_append(str, new_str));
}
/* Similar to 'String_append()', but appends an unsigned integer instead of a character
 * array.
 *
 * Parameters:
 * 		str: The object to modify.
 * 		val: The unsigned integer to append. */
alib_error String_append_uint(String* str, const unsigned long long val)
{
	char new_str[ULLONG_MAX_CHAR_LEN + 1];
	sprintf(new_str, "%llu", val);
	return(String_append(str, new_str));
}
/* Similar to 'String_append()', but appends a double instead of a character
 * array.
 *
 * Parameters:
 * 		str: The object to modify.
 * 		val: The double to append. */
alib_error String_append_double(String* str, const double val)
{
	char new_str[DOUBLE_MAX_CHAR_LEN + 1];
	sprintf(new_str, "%f", val);
	return(String_append(str, new_str));
}
/* Similar to 'String_append()', but appends a boolean instead of a character
 * array.
 *
 * Parameters:
 * 		str: The object to modify.
 * 		val: The value to append, 0 for false, !0 for true.
 * 		upper: If !0, the value will be appended to the string
 * 			in all upper case characters, otherwise all lower case
 * 			characters will be appended. */
alib_error String_append_bool(String* str, size_t val, char upper)
{
	if (val)
		return(String_append_count(str, (upper)?TRUE_STRING_UPPER:TRUE_STRING, TRUE_STRING_LEN));
	else
		return(String_append_count(str, (upper)?FALSE_STRING_UPPER:FALSE_STRING, FALSE_STRING_LEN));
}
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
alib_error String_append_file(String* str, FILE* file)
{
	if(!str)return(ALIB_BAD_ARG);

	if(str->base.len > 0)
		str->base.len -= 1;
	alib_error err = BinaryBuffer_append_file((BinaryBuffer*)str, file);
	if(!err)
		err = BinaryBuffer_append((BinaryBuffer*)str, "\0", 1);
	else
		BinaryBuffer_append((BinaryBuffer*)str, "\0", 1);
	return(err);
}

/* Inserts a c-string at a specific index.
 *
 * Parameters:
 * 		str: The object to modify.
 * 		val: A null-terminated string to insert.  Value is copied.
 * 		index: The index to insert at. This will be the index of the first character
 * 			of 'val'. */
alib_error String_insert(String* str, const char* val, size_t index)
{
	if(val)
		return(String_insert_count(str, val, index, strlen(val)));
	else
		return(ALIB_BAD_ARG);
}
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
		size_t count)
{
	if(!str)return(ALIB_BAD_ARG);

	alib_error err = BinaryBuffer_insert(&str->base, index, val, count);
	if(err)return(err);

	str->base.buff[str->base.len] = 0;
	return(ALIB_OK);
}
/* Same as 'String_insert()' except takes a 'String' object instead of a c-string.
 *
 * Parameters:
 * 		str: The object to modify.
 * 		val: The object to insert.
 * 		index: The index at which 'val' should be inserted. */
alib_error String_insert_string(String* str, const String* val, size_t index)
{
	if(!val)return(ALIB_BAD_ARG);

	return(String_insert_count(str, (char*)val->base.buff, index, val->base.len));
}
/* Same as 'String_insert()' except this inserts an integer.
 *
 * Parameters:
 * 		str: The object to modify.
 * 		val: The value to insert.
 * 		index: The index at which 'val' should be inserted. */
alib_error String_insert_int(String* str, const long long val,
		unsigned int index)
{
	char new_str[LLONG_MAX_CHAR_LEN + 1];
	sprintf(new_str, "%lld", val);
	return(String_insert(str, new_str, index));
}
/* Same as 'String_insert()' except this inserts an unsigned integer.
 *
 * Parameters:
 * 		str: The object to modify.
 * 		val: The value to insert.
 * 		index: The index at which 'val' should be inserted. */
alib_error String_insert_uint(String* str, const unsigned long long val,
		unsigned int index)
{
	char new_str[ULLONG_MAX_CHAR_LEN + 1];
	sprintf(new_str, "%llu", val);
	return(String_insert(str, new_str, index));
}
/* Same as 'String_insert()' except this inserts a double.
 *
 * Parameters:
 * 		str: The object to modify.
 * 		val: The value to insert.
 * 		index: The index at which 'val' should be inserted. */
alib_error String_insert_double(String* str, const double val,
		unsigned int index)
{
	char new_str[DOUBLE_MAX_CHAR_LEN + 1];
	sprintf(new_str, "%f", val);
	return(String_insert(str, new_str, index));
}
/* Same as 'String_insert()' except this inserts a boolean.
 *
 * Parameters:
 * 		str: The object to modify.
 * 		val: The value to insert.
 * 		index: The index at which 'val' should be inserted. */
alib_error String_insert_bool(String* str, const unsigned char val,
		unsigned int index, char upper)
{
	if (val)
		return(String_insert_count(str, (upper)?TRUE_STRING_UPPER:TRUE_STRING, index, TRUE_STRING_LEN));
	else
		return(String_insert_count(str, (upper)?FALSE_STRING_UPPER:FALSE_STRING, index, FALSE_STRING_LEN));
}

/* Copies one String to another String.
 *
 * Parameters:
 * 		to: The String to copy to.
 * 		from: The String to copy from. */
alib_error String_copy_string(String* to, const String* from)
{
	if(!to || !from)return(ALIB_BAD_ARG);

	String_clear(to);
	to->base.max_expand = from->base.max_expand;
	to->base.min_cap = from->base.min_cap;

	BinaryBuffer_resize(&to->base, from->base.capacity);
	memcpy(to->base.buff, from->base.buff, from->base.len);
	to->base.len = from->base.len;

	return(ALIB_OK);
}

/* Changes all the characters in 'str's buffer to lower case. */
void String_to_lower(String* str)
{
	if(!str || !str->base.buff)return;

	str_to_lower((char*)str->base.buff);
}
/* Changes all the characters in 'str's buffer to upper case. */
void String_to_upper(String* str)
{
	if(!str || !str->base.buff)return;

	str_to_upper((char*)str->base.buff);
}

/* Shrinks a string's capacity to fit the number of bytes
 * actually used. */
alib_error String_shrink_to_fit(String* str)
{
	if(!str)return(ALIB_BAD_ARG);

	BinaryBuffer_shrink_to_fit(&str->base);
	return(ALIB_OK);
}

/* Removes a portion of a string from 'begin' to 'end'.
 *
 * Parameters:
 * 		str: The object to modify.
 * 		begin: The index to start removing from (inclusive).
 * 		end: The index to stop removing from (exclusive).  */
alib_error String_remove(String* str, size_t begin, size_t end)
{
	if(!str)return(ALIB_BAD_ARG);

	if(begin > end)
	{
		size_t tmp = end;
		end = begin;
		begin = tmp;
	}
	if(begin >= String_get_length(str))
		return(ALIB_OK);

	if(end >= str->base.len)
		end = str->base.len - 1;

	return(BinaryBuffer_remove(&str->base, begin, end));
}
/* Removes all matching substrings found in 'str'.
 *
 * Parameters:
 * 		str: The object to modify.
 * 		substring: The substrings to search for that are to be removed.
 * 			THIS MUST BE NULL TERMINATED, otherwise behavior is undefined.
 * 		substring_len: (OPTIONAL) The length of the substring. */
alib_error String_remove_substring(String* str, const char* substring)
{
	if(!str || !substring)return(ALIB_BAD_ARG);

	size_t index;
	char* ptr;
	size_t substring_len = strlen(substring);

	ptr = strstr((char*)str->base.buff, substring);
	for(; ptr; ptr = strstr(ptr, substring))
	{
		index = (size_t)(ptr - (char*)str->base.buff);
		String_remove(str, index, index + substring_len);
	}

	return(ALIB_OK);
}

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
alib_error String_replace(String* str, size_t index, long char_replace_count, const char* substring)
{
	if(!substring)return(ALIB_BAD_ARG);
	return(String_replace_count(str, index, char_replace_count, substring, strlen(substring)));
}
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
alib_error String_replace_count(String* str, size_t index, int char_replace_count, const char* substring,
		size_t count)
{
	if(!str || !substring || index >= str->base.len)return(ALIB_BAD_ARG);

	if(char_replace_count < 0)
		char_replace_count = count;

	alib_error err = BinaryBuffer_replace(&str->base, index, char_replace_count, substring, count);
	if(err)return(err);

	str->base.buff[str->base.len] = 0;
	return(ALIB_OK);
}

/* Searches for a substring in 'str' that matches 'substring' and replaces it with 'newstr'.
 *
 * Parameters:
 * 		str: The object to modify.
 * 		substring: The substring to replace.  MUST be null terminated.
 * 		newstr: The string to replace 'substring' with.
 * 		newstr_len: (OPTIONAL) The length of 'newstr'. */
alib_error String_replace_substrings(String* str, const char* substring,
		const char* newstr, size_t newstr_len)
{
	size_t substring_len;

	if(!str || !substring || !newstr)return(ALIB_BAD_ARG);

	substring_len = strlen(substring);
	if(!newstr_len)
		newstr_len = strlen(newstr);

	alib_error err;
	size_t index;
	char* str_ptr = strstr((char*)str->base.buff, substring);
	for(; str_ptr; str_ptr = strstr(str_ptr, substring))
	{
		index = (size_t)((str_ptr + substring_len) - (char*)str->base.buff);

		err = BinaryBuffer_replace((BinaryBuffer*)str,
				(size_t)((size_t)str_ptr - (size_t)str->base.buff),
				substring_len, newstr, newstr_len);
		if(err)return(err);

		str_ptr = (char*)(str->base.buff + index);
	}

	return(ALIB_OK);
}

/* Counts the number of matching substrings in the given String.
 *
 * Parameters:
 * 		str: The object search.
 * 		substring: The string to search for.
 *
 * Returns:
 * 		 <0: alib_error
 * 		0>=: Number of substrings found. */
long String_count_substrings(const String* str, const char* substring)
{
	if(!str || !substring)return(ALIB_BAD_ARG);

	long count = 0;
	size_t substring_len = strlen(substring);

	char* it = (char*)str->base.buff;
	for(; (it = strstr(it, substring)); ++count, it += substring_len);

	return(count);
}

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
char* const* String_get_substring_ptrs(const String* str, const char* substring)
{
	if(!str || !substring)return(NULL);

	char* str_it, **rval;
	size_t substring_len = strlen(substring);
	BinaryBuffer* ptr_buff = newBinaryBuffer();
	if(!ptr_buff)return(NULL);

	str_it = (char*)str->base.buff;
	for(; (str_it = strstr(str_it, substring)); str_it += substring_len)
		BinaryBuffer_append(ptr_buff, &str_it, sizeof(str_it));

	str_it = NULL;
	BinaryBuffer_append(ptr_buff, &str_it, sizeof(str_it));
	BinaryBuffer_shrink_to_fit(ptr_buff);
	rval = BinaryBuffer_extract_buffer(ptr_buff);
	delBinaryBuffer(&ptr_buff);

	return(rval);
}

/* Returns the internal buffer and clears the String.
 * The returned value is no longer handled by the String object and
 * therefore must be freed by the caller.
 *
 * Returns:
 * 		NULL: Either no internal memory is allocated by the String,
 * 			or an error occurred.
 * 		char*: The internal buffer of the String. */
char* String_extract_c_string(String* str)
{
	if(!str)return(NULL);

	return((char*)BinaryBuffer_extract_buffer(&str->base));
}

	/* Getters */
/* Returns a pointer to the internal c-string.  The returned value should not be modified nor
 * freed.
 *
 * Assumes 'str' is not null. */
const char* String_get_c_string(String* str){return((const char*)str->base.buff);}
/* Returns the length of the String.
 *
 * Assumes 'str' is not null. */
size_t String_get_length(String* str){return((str->base.len)?str->base.len - 1:0);}
/* Returns the number of bytes allocated by the internal buffer.
 *
 * Assumes 'str' is not null. */
size_t String_get_capacity(String* str){return(str->base.capacity);}
/* Returns the index of the given pointer within the string.
 *
 * Returns:
 * 		0>=: Index of the pointer.
 * 		 <0: The given pointer does not belong to the String. */
long String_get_index(String* str, const char* ptr)
{
	if(((size_t)ptr > (size_t)str->base.buff) &&
			(size_t)ptr < (size_t)(str->base.buff + str->base.len))
		return((long)((long)ptr - (long)str->base.buff));
	else
		return(ALIB_BAD_ARG);
}
	/***********/
/******************************/

/*******Lifecycle*******/
	/* Constructors */
/* Default constructor. */
String* newString()
{
	return((String*)newBinaryBuffer());
}
/* Copy constructor. */
String* newString_copy(String* str)
{
	String* new_str = newString();
	if(!new_str)return(NULL);

	new_str->base.max_expand = str->base.max_expand;
	new_str->base.min_cap = str->base.min_cap;

	BinaryBuffer_resize(&new_str->base, str->base.capacity);
	memcpy(new_str->base.buff, str->base.buff, str->base.len);

	return(new_str);
}
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
String* newString_ex(const char* str, size_t len)
{
	String* rval = newString();

	if(!len)len = strlen(str);
	if(String_append_count(rval, str, len))
		delString(&rval);

	return(rval);
}
	/****************/

	/* Destructors */
/* Default destructor. */
void delString(String** str)
{
	delBinaryBuffer((BinaryBuffer**)str);
}
	/***************/
/***********************/
