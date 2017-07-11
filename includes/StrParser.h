#ifndef STR_PARSER_IS_DEFINED
#define STR_PARSER_IS_DEFINED

#include "alib_error.h"
#include "alib_types.h"
#include "StrRef.h"

/* Structs */
/* String parser is designed to be a base interface for parsers each string may need to be
 * parsed differently. */
typedef struct StrParser StrParser;
/***********/

/* Public Functions */
/* Initializes an StrParser.  This MUST BE CALLED before utilizing a parser. */
alib_error StrParser_init(StrParser* parser, const char* str, const char* strEnd);
/* Same as 'StrParser_init(parser, str, str + strLen)'. */
alib_error StrParser_init_w_size(StrParser* parser, const char* str, size_t strLen);

/* Resets the iterator inside the parser. */
alib_error StrParser_reset(StrParser* parser);
/* Trims a set of characters from the beginning and end of the string.
 *
 * Parameters:
 * 		parser: The parser to trim.
 * 		chars: 	The array of characters to trim.
 * 					If no null terminator is provided for this array, then 'count' must reflect the number of characters in this array.
 * 		count: 	The number of characters in 'chars'.
 * 			   		If zero, then 'chars' will be counted to the null terminator. */
alib_error StrParser_trim(StrParser* parser, const char* chars, size_t count);
/* Trims whitespaces from the beginning and end of the string. */
alib_error StrParser_trim_whitespaces(StrParser* parser);

	/* Getters */
/* Returns the length of the string being parsed. */
size_t StrParser_get_str_len(StrParser* parser);
/* Returns the length of iterator being parsed. */
size_t StrParser_get_it_len(StrParser* parser);
	/***********/
/********************/

/* Private Functions */

/*********************/

/* TODO: Add dynamic constructors. */

#endif
