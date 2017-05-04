#include "StrParser.h"

/* Public Functions */
/* Initializes an StrParser.  This MUST BE CALLED before utilizing a parser. */
alib_error StrParser_init(StrParser* parser, const char* str, const char* strEnd)
{
	if(!parser || !str || !strEnd || str > strEnd)return(ALIB_BAD_ARG);

	parser->strBegin = str;
	parser->strEnd = strEnd;

	return(StrParser_reset(parser));
}
/* Same as 'StrParser_init(parser, str, str + strLen)'. */
alib_error StrParser_init_w_size(StrParser* parser, const char* str, size_t strLen)
{
	return(StrParser_init(parser, str, str + strLen));
}

/* Resets the iterator inside the parser. */
alib_error StrParser_reset(StrParser* parser)
{
	if(!parser)return(ALIB_BAD_ARG);

	parser->itBegin = NULL;
	parser->itEnd = parser->strBegin;

	return(ALIB_OK);
}
/* Trims a set of characters from the beginning and end of the string.
 *
 * Parameters:
 * 		parser: The parser to trim.
 * 		chars: 	The array of characters to trim.
 * 					If no null terminator is provided for this array, then 'count' must reflect the number of characters in this array.
 * 		count: 	The number of characters in 'chars'.
 * 			   		If zero, then 'chars' will be counted to the null terminator. */
alib_error StrParser_trim(StrParser* parser, const char* chars, size_t count)
{
	if(!parser)return(ALIB_BAD_ARG);

	const char* strIt;
	const char* cit, *citEnd;

	cit = chars;
	if(!count)
	{
		citEnd = chars;
		for(; *citEnd; ++citEnd);
	}
	else
		citEnd = chars + count;

	/* Trim beginning of string. */
	for(strIt = parser->strBegin; strIt < parser->strEnd; ++strIt)
	{
		for(cit = chars; *cit != *strIt; ++cit);

		/* We will break if there were no characters found that matched to trim. */
		if(cit == citEnd)
			break;
	}
	parser->strBegin = strIt;

	/* Trim end of string. */
	for(strIt = parser->strEnd - 1; strIt > parser->strBegin; --strIt)
	{
		for(cit = chars; *cit != *strIt; ++cit);

		/* We will break if there were no characters found that matched to trim. */
		if(cit == citEnd)
			break;
	}
	parser->strEnd = strIt + 1;	/* strEnd should point to one past end of string. */

	if(parser->itBegin < parser->strBegin)
		StrParser_reset(parser);

	return(ALIB_OK);
}
/* Trims whitespaces from the beginning and end of the string. */
alib_error StrParser_trim_whitespaces(StrParser* parser)
{
	if(!parser)return(ALIB_BAD_ARG);

	const char whitespaces[] = {' ', '\t', '\r', '\n'};

	return(StrParser_trim(parser, whitespaces, sizeof(whitespaces)));
}

	/* Getters */
/* Returns the length of the string being parsed. */
size_t StrParser_get_str_len(StrParser* parser)
{
	if(!parser)
		return(0);
	else
		return(parser->strEnd - parser->strBegin);
}
/* Returns the length of iterator being parsed. */
size_t StrParser_get_it_len(StrParser* parser)
{
	if(!parser || !parser->itBegin)
		return(0);
	else
		return(parser->itEnd - parser->itBegin);
}
	/***********/
/********************/

/* Private Functions */

/*********************/

/* TODO: Move to C file. */
/* TODO: Add dynamic constructors. */
