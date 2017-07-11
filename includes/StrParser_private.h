#ifndef ALIB_C_STR_PARSER_PRIVATE_IS_DEFINED
#define ALIB_C_STR_PARSER_PRIVATE_IS_DEFINED

#include "StrParser.h"
#include "StrRef_private.h"

/* Structs */
/* String parser is designed to be a base interface for parsers each string may need to be
 * parsed differently. */
struct StrParser
{
	/* Pointer to the entire string being parsed. */
	StrRef str;

	/* Pointer to the beginning of an iterator segment of the string being parsed.
	 * If 'it.begin' is NULL, then the iterator has not been initialized yet. */
	StrRef it;
};
/***********/

#endif
