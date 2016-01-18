#include "StringObject.h"
#include "alib_error.h"
#include "alib_string.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/*TODO:
 * 		ADD 16 AND 32 BIT CHARACTER SUPPORT.*/

/*******Private Functions*******/
/* Private function that will resize the allocated memory to the
 * desired size.  If new memory has to be allocated, the string's contents
 * will be copied to the newly allocated memory.
 *
 * Return Codes:
 * 		ALIB_MEM_ERR
 * 		ALIB_BAD_ARG
 * 		ALIB_OK
 */
static char StringObject_resize(StringObject* str, unsigned int size)
{
	if (str == NULL)
		return(ALIB_BAD_ARG);

	//If the size is zero, then just free memory and set the string to null.
	if (size == 0)
	{
		str->str = realloc(str->str, ++size);
		if(!str->str)
			return(ALIB_MEM_ERR);

		*(str->str) = 0;
		str->capacity = 1;
		str->length = 0;
		return(ALIB_OK);
	}

	str->capacity = size;
	/* Ensure the first character in the string is null if the string was null to
	 * begin with.*/
	if(str->str != NULL)
	{
		str->str = realloc(str->str, size);
		if(!str->str)
			return(ALIB_MEM_ERR);
	}
	else
	{
		str->str = realloc(str->str, size);
		if(!str->str)
			return(ALIB_MEM_ERR);

		*(str->str) = '\0';
	}

	return(ALIB_OK);
}
/* Finds the amount of memory to allocate, then resizes the string.
 *
 * Parameters:
 * 		str: String object to work on.
 * 		min_size: The minimum size that the string must be. This MUST
 * 			include the null terminator.
 *
 * Return Codes:
 * 		ALIB_MEM_ERR
 * 		ALIB_BAD_ARG
 * 		ALIB_OK
 */
static char StringObject_calc_and_resize(StringObject* str, unsigned int min_size)
{
	if(!str)
		return(ALIB_BAD_ARG);

	//If the min_size is zero, then we need to clear the string.
	if(min_size == 0)
		StringObject_clear(str);
	//If the current size is way too big, then we shrink
	else if((min_size < str->capacity) &&
			((min_size < 1024 && min_size * 4 < str->capacity) ||
			(min_size + 4 * 1024 < str->capacity)))
	{
		StringObject_resize(str, min_size);
	}
	else
	{
		while(str->capacity < min_size)
		{
			//If we have not allocated any memory, set it to a minimum size.
			if(str->capacity == 0)
				str->capacity = (min_size < str->min_mem)?str->min_mem:min_size;
			//If the current capacity is less than 64k, continue doubling it.
			else if(str->capacity < 64 * 1024)
				str->capacity *= 2;
			//Otherwise, only append 32k data per iteration.
			else
				str->capacity += 32 * 1024;
		}
		StringObject_resize(str, str->capacity);
	}

	return(ALIB_OK);
}
/*******************************/

/*******Object Functions*******/
/* Sets the value of the StringObject to the given c-string.
 *
 * Return Codes:
 * 		ALIB_MEM_ERR
 * 		ALIB_BAD_ARG
 * 		ALIB_OK
 */
char StringObject_set(StringObject* str, const char* str_value)
{
	unsigned int n_str_len;
	char r_code;

	//Ensure StringObject is not null
	if (!str)
		return(ALIB_BAD_ARG);

	//If the given value is null, we clear the string
	if(!str_value)
	{
		//Free the memory if it is more than the minimum size
		StringObject_clear(str);
		return(ALIB_OK);
	}

	//Calculate the new string's length.
	n_str_len = strlen(str_value);

	//If the current length is not large enough or it is too large, resize.
	if(n_str_len > str->length || str->length - n_str_len > str->min_mem * 4)
	{
		r_code = StringObject_calc_and_resize(str, n_str_len + 1);
		if(r_code)return(r_code);
	}

	//Copy the string.
	strcpy(str->str, str_value);
	str->length = n_str_len;

	return(ALIB_OK);
}
/* Sets the value of the StringObject to the given c-string of size 'count'.
 *
 * Return Codes:
 * 		ALIB_MEM_ERR
 * 		ALIB_BAD_ARG
 * 		ALIB_OK
 */
char StringObject_set_count(StringObject* str, const char* str_value, unsigned int count)
{
	char r_code;
	unsigned int n_str_len;

	//Ensure StringObject is not null
	if (!str || (!str_value && count > 0))
		return(ALIB_BAD_ARG);

	//If the given value is null, we clear the string
	if(count == 0)
	{
		//Free the memory if it is more than the minimum size
		StringObject_clear(str);
		return(ALIB_OK);
	}

	n_str_len = count;

	/* Check to see if the string is null_terminated,
	 * if it's not, add one to n_str_len for the null termination.
	 */
	if(*(str_value + count) != 0)
		++n_str_len;

	//If the current length is not large enough or it is too large, resize.
	if(n_str_len > str->length || str->length - n_str_len > str->min_mem * 4)
	{
		r_code = StringObject_calc_and_resize(str, n_str_len);
		if(r_code)return(r_code);
	}

	//Copy the string.
	strncpy(str->str, str_value, count);

	//Ensure there is a null terminator
	*(str->str + n_str_len) = 0;
	str->length = n_str_len;

	return(ALIB_OK);
}

/* Clears the memory that has been allocated.  If the memory is
 * larger than the minimum size, then the memory will be freed.
 * If freeing memory is not desired, use StringObject_clear_no_free() instead.
 */
void StringObject_clear(StringObject* str)
{
	if (str == NULL)
		return;

	if (str->str != NULL && str->capacity > str->min_mem)
		StringObject_resize(str, 0);
	else
	{
		*(str->str) = 0;
		str->length = 0;
	}
}
/* Clears the string, by setting the first character in the
 * string to null, but does not free any memory.
 */
void StringObject_clear_no_free(StringObject* str)
{
	if(str == NULL)
		return;

	//Set the value to zero.
	*(str->str) = 0;
	str->length = 0;
}

/* Appends a null terminated c-string into the StringObject.
 * If 'str_to_append' is not null terminated, results are undefined.
 *
 * Return Codes:
 * 		ALIB_MEM_ERR
 * 		ALIB_BAD_ARG
 * 		ALIB_OK
 */
char StringObject_append(StringObject* str, const char* str_to_append)
{
	int t_len ;
	char r_code;

	if (str == NULL || !str_to_append)
		return(ALIB_BAD_ARG);

	//Find the new length of the string, add 1 for null terminator
	t_len = str->length + strlen(str_to_append) + 1;

	//Resize if needed
	r_code = StringObject_calc_and_resize(str, t_len);
	if(r_code)return(r_code);

	//Copy the appended string over
	strcpy((str->str + str->length), str_to_append);
	str->length = t_len - 1;

	return(ALIB_OK);
}
/* Copies a c-string onto the end of a StringObject.  It will copy up to and not exceeding
 * the size of the 'count' arg.  If the 'count' arg is zero, function will return ALIB_OK.
 *
 * Return Codes:
 * 		ALIB_MEM_ERR
 * 		ALIB_BAD_ARG
 * 		ALIB_OK
 */
char StringObject_append_count(StringObject* str, const char* str_to_append, unsigned int count)
{
	unsigned int t_len, calc_len;
	char r_code;

	if(!str || !str_to_append)
		return(ALIB_BAD_ARG);

	//If the given count is zero, return.
	if(count == 0)
		return(ALIB_OK);

	//This does not include the null terminator.
	t_len = str->length + count;

	/* If the new string does not have a null-terminator, then we need to
	 * add one to our length. */
	if(*(str_to_append + count - 1) != '\0')
		++t_len;

	//Resize if needed
	r_code = StringObject_calc_and_resize(str, t_len);
	if(r_code)return(r_code);

	//Copy the string over.
	strncpy((str->str + str->length), str_to_append, count);

	//Set the string's count.
	calc_len = strlen(str_to_append);
	if(calc_len < count)
		str->length = str->length + calc_len;
	else
	{
		str->length = t_len - 1; //Subtract the null terminator
		*(str->str + str->length) = 0;  //Set the null terminator
	}

	return(ALIB_OK);
}
/* Appends a single character to the end of the string.
 *
 * Return Codes:
 * 		STRING_OBJECT_MEM_ERR
 * 		STRING_OBJECT_BAD_ARG
 * 		STRING_OBJECT_OK
 */
char StringObject_append_char(StringObject* str, char c)
{
	if(!str)return(ALIB_BAD_ARG);
	return(str->append_count(str, &c, 1));
}
/* Parses an integer into a string then appends it to the StringObject.
 *
 * Return Codes:
 * 		ALIB_MEM_ERR
 * 		ALIB_BAD_ARG
 * 		ALIB_OK
 */
char StringObject_append_int(StringObject* str, const long long val)
{
	char new_str[LLONG_MAX_CHAR_LEN + 1];
	sprintf(new_str, "%lld", val);
	return(StringObject_append(str, new_str));
}
/* Parses an unsigned integer into a string then appends it to the StringObject.
 *
 * Return Codes:
 * 		ALIB_MEM_ERR
 * 		ALIB_BAD_ARG
 * 		ALIB_OK
 */
char StringObject_append_uint(StringObject* str, const unsigned long long val)
{
	char new_str[ULLONG_MAX_CHAR_LEN + 1];
	sprintf(new_str, "%llu", val);
	return(StringObject_append(str, new_str));
}
/* Parses a double into a string then appends it to the StringObject.
 *
 * Return Codes:
 * 		ALIB_MEM_ERR
 * 		ALIB_BAD_ARG
 * 		ALIB_OK
 */
char StringObject_append_double(StringObject* str, const double val)
{
	char new_str[DOUBLE_MAX_CHAR_LEN + 1];
	sprintf(new_str, "%f", val);
	return(StringObject_append(str, new_str));
}
/* Parses an boolean into a string then appends it to the StringObject.
 *
 * Return Codes:
 * 		ALIB_MEM_ERR
 * 		ALIB_BAD_ARG
 * 		ALIB_OK
 */
char StringObject_append_bool(StringObject* str, const unsigned char val)
{
	if (val)
		return(StringObject_append(str, "true"));
	else
		return(StringObject_append(str, "false"));
}

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
char StringObject_insert(StringObject* str, const char* val, unsigned int index)
{
	return(StringObject_insert_count(str, val, index, strlen(val)));
}
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
		unsigned int val_len)
{
	char err = 0;
	unsigned int val_real_size = 0;
	const char* val_it = val;

	/* Ensure that the real size of the string does not exceed the given size. */
	for(;*val_it != '\0' && val_real_size < val_len;
			++val_it, ++val_real_size);
	if(val_real_size < val_len)
		val_len = val_real_size;

	/* Resize the string as needed. */
	if((err = StringObject_calc_and_resize(str, str->length + val_len + 1)))
		return(err);

	/* Move the data to the new position. */
	strncpy_back(str->str + index + val_len, str->str + index, str->length - index);

	/* Add new data to desired index. */
	strncpy(str->str + index, val, val_len);

	/* Modify property members. */
	str->length += val_len;
	str->str[str->length] = '\0';

	return(err);
}
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
		unsigned int index)
{
	char new_str[LLONG_MAX_CHAR_LEN + 1];
	sprintf(new_str, "%lld", val);
	return(StringObject_insert(str, new_str, index));
}
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
		unsigned int index)
{
	char new_str[ULLONG_MAX_CHAR_LEN + 1];
	sprintf(new_str, "%llu", val);
	return(StringObject_insert(str, new_str, index));
}
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
		unsigned int index)
{
	char new_str[DOUBLE_MAX_CHAR_LEN + 1];
	sprintf(new_str, "%f", val);
	return(StringObject_insert(str, new_str, index));
}
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
		unsigned int index)
{
	if (val)
		return(StringObject_insert(str, "true", index));
	else
		return(StringObject_insert(str, "false", index));
}

/* Replaces all characters in the given StringObject with lower-case
 * equivalents.  If modifying the given string is not desired, please
 * use StringObject_to_lower_copy(StringObject*).*/
void StringObject_to_lower(StringObject* str)
{
	char* c;

	if(!str)return;

	for(c = str->str; *c != '\0'; *c = tolower(*c), ++c);
}
/* Does the same thing as StringObject_to_lower(StringObject*) except
 * that the given StringObject is not modified and it returns a
 * dynamically allocated character string of the lowered string.
 *
 * #REMEMBER to free the returned c-string.
 */
char* StringObject_to_lower_copy(StringObject* str)
{
	char* c_it, *r_str, *r_ptr;

	if(!str)return(NULL);

	r_str = malloc(sizeof(char) * (str->length + 1));
	for(c_it = str->str, r_ptr = r_str; *c_it != '\0'; ++c_it, ++r_ptr)
		*r_ptr = tolower(*c_it);
	*r_ptr = *c_it;
	return(r_str);
}
/* Replaces all characters in the given StringObject with upper-case
 * equivalents.  If modifying the given string is not desired, please
 * use StringObject_to_upper_copy(StringObject*).*/
void StringObject_to_upper(StringObject* str)
{
	char* c;

	if(!str)return;

	for(c = str->str; *c != '\0'; *c = toupper(*c), ++c);
}
/* Does the same thing as StringObject_to_upper(StringObject*) except
 * that the given StringObject is not modified and it returns a
 * dynamically allocated character string of the uppered string.
 *
 * #REMEMBER to free the returned c-string.
 */
char* StringObject_to_upper_copy(StringObject* str)
{
	char* c_it, *r_str, *r_ptr;

	if(!str)return(NULL);

	r_str = malloc(sizeof(char) * (str->length + 1));
	for(c_it = str->str, r_ptr = r_str; *c_it != '\0'; ++c_it, ++r_ptr)
		*r_ptr = toupper(*c_it);
	*r_ptr = *c_it;
	return(r_str);
}

/* Shrinks the c-string to fit the size of its contents.
 *
 * Return Codes:
 * 		ALIB_MEM_ERR
 * 		ALIB_BAD_ARG
 * 		ALIB_OK
 */
char StringObject_shrink_to_fit(StringObject* str)
{
	if(!str)
		return(ALIB_BAD_ARG);

	return(StringObject_resize(str, str->length + 1));	//Add one for null terminator.
}

/* Replaces all the substrings that matches 'substring' with 'newstr'.
 * If 'newstr' is null, then the 'substring's will be removed. */
StringObject* StringObject_replace_substrings(StringObject* str, const char* substring,
		const char* newstr)
{
	int i = str->length, substr_len, substr_counter;
	StringObject* r_str = newStringObject();
	char* ibegin, *c_it;
	const char* substr_ptr;

	if(!str || !str->str || !substring || !r_str)return(str);
	substr_len = strlen(substring);

	//Replace all 'substring's with 'newstr's, placing data in
	//'r_str' along the way.
	for(c_it = ibegin = str->str, substr_counter = 0;i > 0;
			)
	{
		if(*c_it == *substring)
		{
			int j = 0;
			for(substr_ptr = substring; j < substr_len && *c_it == *substr_ptr;
					++j, --i, ++c_it, ++substr_ptr);

			//Found a matching substring
			if(j == substr_len)
			{
				r_str->append_count(r_str, ibegin, substr_counter);
				if(newstr)
					r_str->append(r_str, newstr);
				ibegin = c_it;
				substr_counter = 0;
			}
			else
				substr_counter += j;
		}
		else
		{
			--i;
			++c_it;
			++substr_counter;
		}
	}
	if(substr_counter)
		r_str->append_count(r_str, ibegin, substr_counter);

	/* Transfer the data from r_str to str, this requires some
	 * complex object member fenagling to increase efficiency. */
	if(str->str != NULL)
		free(str->str);
	str->capacity = r_str->capacity;
	str->length = r_str->length;
	str->str = StringObject_extract_c_string(r_str);

	delStringObject(&r_str);
	return(str);
}

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
char* StringObject_extract_c_string(StringObject* str)
{
	char* r_str;

	if(!str)return(NULL);
	r_str = str->str;

	str->str = NULL;
	str->length = 0;
	if(StringObject_resize(str, str->min_mem))
	{
		str->str = r_str;
		return(NULL);
	}

	return(r_str);
}
/******************************/

/*Constructors*/
/* Creates a new StringObject with default values.
 * The c-string will be set to null and will not be allocated
 * until a function is called.
 *
 * Returns null if memory could not be allocated.
 */
StringObject* newStringObject()
{
	StringObject* rString = (StringObject*)malloc(sizeof(StringObject));
	if(!rString)
		return(NULL);

	//Build members
	rString->str = (char*)malloc(sizeof(char));	//Need to set this first...
	if(!rString->str)
	{
		free(rString);
		return(NULL);
	}
	*(rString->str) = 0;

	rString->length = 0;
	rString->capacity = 1;
	rString->min_mem = 64;

#if !STRING_OBJECT_MIN_MEMORY
	//Setup function pointers
	rString->set = StringObject_set;
	rString->set_count = StringObject_set_count;
	rString->clear = StringObject_clear;

	rString->append = StringObject_append;
	rString->append_count = StringObject_append_count;
	rString->append_bool = StringObject_append_bool;
	rString->append_double = StringObject_append_double;
	rString->append_int = StringObject_append_int;
	rString->append_uint = StringObject_append_uint;
	rString->insert = StringObject_insert;
	rString->insert_count = StringObject_insert_count;
	rString->to_lower = StringObject_to_lower;
	rString->to_upper = StringObject_to_upper;
	rString->shrink_to_fit = StringObject_shrink_to_fit;
#endif

	return(rString);
}

/* Deallocates all memory used by the string object and sets the pointer to null. */
void delStringObject(StringObject** str)
{
	if (!str || !*str)
		return;

	free((*str)->str);
	free(*str);
	*str = NULL;
}
/**************/
