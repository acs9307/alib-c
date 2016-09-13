#include "alib_types.h"

/*******Structs*******/
	/* Wrapper Package */
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
		alib_free_value free_data_cb)
{
	wrapper_package* package = (wrapper_package*)malloc(sizeof(wrapper_package));
	if(!package)return(NULL);

	package->parent = parent;
	package->data = data;
	package->free_data = free_data_cb;

	return(package);
}
		/****************/

		/* Destructors */
/* Frees a wrapper package. */
void free_wrapper_package(wrapper_package* package)
{
	if(!package)return;

	if(package->data && package->free_data)
		package->free_data(package->data);
	free(package);
}
/* Deletes a wrapper package and sets the pointer to null. */
void del_wrapper_package(wrapper_package** package)
{
	if(!package)return;

	free_wrapper_package(*package);
	*package = NULL;
}
		/***************/

		/* Destructor Callbacks */
/* Callback function that is of type 'alib_free_value'.
 *
 * Calls 'free_wrapper_package()'. */
void free_wrapper_package_cb(void* r_package)
{
	free_wrapper_package((wrapper_package*)r_package);
}
/* Callback function that is of type 'alib_free_value'.
 *
 * Calls 'del_wrapper_package()'. */
void del_wrapper_package_cb(void** r_package)
{
	del_wrapper_package((wrapper_package**)r_package);
}
		/************************/
	/*******************/
/*********************/

/*******Functions*******/
/* Returns true if the character is a whitespace,
 * otherwise returns false. */
char is_whitespace(char c)
{
	if(c == ' ' || c == '\n' || c == '\t' || c == '\r')
		return(1);
	else
		return(0);
}

/* Returns true if the null-terminated string is a valid integer.
 *
 * White spaces at beginning are ignored.  Function stops
 * checking the string after the second set of white-spaces are hit,
 * or a null terminator is hit.
 */
char is_int(const char* str)
{
	char r_val = 0;

	//Skip front end whitespaces
	while(is_whitespace(*str))
		++str;

	//Continue till end of string or a whitespace is hit
	while(*str != '\0' && !is_whitespace(*str))
	{
		//Non-integer value, return false.
		if(*str < '0' || *str > '9')
			return(0);
		else
		{
			r_val = 1;
			++str;
		}
	}

	return(r_val);
}
/* Returns true if the string is a valid integer.
 *
 * White spaces at beginning are ignored.  Function stops
 * checking the string after the second set of white-spaces are hit,
 * or a null terminator is hit.
 */
char is_int_n(const char* str, int len)
{
	char r_val = 0;

	//Skip over front whitespaces
	for(; len > 0 && is_whitespace(*str); --len, ++str);

	//Search until a whitespace is hit or the length is zero
	for(; len > 0 && !is_whitespace(*str); --len, ++str)
	{
		if(*str < '0' || *str > '9')
			return(0);
		else
			r_val = 1;
	}

	return(r_val);
}

/* Returns true if the null-terminated string is a valid double.
 *
 * White spaces at beginning are ignored.  Function stops
 * checking the string after the second set of white-spaces are hit,
 * or a null terminator is hit.
 */
char is_double(const char* str)
{
	char r_val = 0, dot_found = 0;

	//Skip front end whitespaces
	while(is_whitespace(*str))
		++str;

	//Continue till end of string or a whitespace is hit
	for(;*str != '\0' && !is_whitespace(*str);++str)
	{
		if(!dot_found && *str == '.')
			dot_found = 1;
		//Non-double value, return false.
		if(*str < '0' || *str > '9')
			return(0);
		else
			r_val = 1;
	}

	return(r_val);
}
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
char is_double_n(const char* str, size_t len)
{
	char r_val = 0, dot_found = 0;

	//Skip front end whitespaces
	for(; len > 0 && is_whitespace(*str); --len, ++str);

	//Continue till end of string or a whitespace is hit
	for(; len > 0 && *str != '\0' && !is_whitespace(*str); --len, ++str)
	{
		if(!dot_found && *str == '.')
			dot_found = 1;
		//Non-double value, return false.
		if(*str < '0' || *str > '9')
			return(0);
		else
			r_val = 1;
	}

	return(r_val);
}

/* Returns true if the string can be parsed into a boolean.
 *
 * White spaces at beginning are ignored.  Function stops
 * checking the string after the second set of white-spaces are hit,
 * or a null terminator is hit.
 */
char is_bool_n(const char* str, size_t str_len)
{
	//Skip front end whitespaces
	while(is_whitespace(*str))
		++str;

	if(str_len < TRUE_STRING_LEN)return(0);

	if(strncasecmp(str, TRUE_STRING, TRUE_STRING_LEN) == 0 && is_whitespace(str[str_len]))
		return(1);
	else if(strncasecmp(str, FALSE_STRING, FALSE_STRING_LEN) == 0 &&
			is_whitespace(str[str_len]))
		return(1);
	else
		return(0);
}
/* Returns true if the null-terminated string can be parsed into a boolean.
 *
 * White spaces at beginning are ignored.  Function stops
 * checking the string after the second set of white-spaces are hit,
 * or a null terminator is hit.
 */
char is_bool(const char* str){return(is_bool_n(str, strlen(str)));}

/* Returns the type that the null-terminated string
 * can be parsed into. */
string_type get_string_type(const char* str)
{
	if(is_int(str))
		return(STRING_TYPE_INT);
	else if(is_double(str))
		return(STRING_TYPE_DOUBLE);
	else if(is_bool(str))
		return(STRING_TYPE_BOOL);
	else
		return(STRING_TYPE_STRING);
}
/* Returns the type that the string can be parsed into. */
string_type get_string_type_n(const char* str, int len)
{
	if(is_int_n(str, len))
		return(STRING_TYPE_INT);
	else if(is_double_n(str, len))
		return(STRING_TYPE_DOUBLE);
	else if(is_bool_n(str, len))
		return(STRING_TYPE_BOOL);
	else
		return(STRING_TYPE_STRING);
}
/***********************/
