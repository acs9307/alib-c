#include "includes/StrParser_private.h"

/* Public Functions */
/* Initializes an StrParser.  This MUST BE CALLED before utilizing a parser. */
alib_error StrParser_init(StrParser* parser, const char* str, const char* strEnd)
{
	if(!parser || !str || !strEnd || str > strEnd)return(ALIB_BAD_ARG);

	parser->str.begin = str;
	parser->str.end = strEnd;

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

	parser->it.begin = NULL;
	parser->it.end = parser->str.begin;

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

	/* Determine the last character in the 'chars' list. */
	if(!count)
	{
		citEnd = chars;
		for(; *citEnd; ++citEnd);
	}
	else
		citEnd = chars + count;

	/* Trim beginning of string. */
	for(strIt = parser->str.begin; strIt < parser->str.end; ++strIt)
	{
		for(cit = chars; *cit != *strIt && cit != citEnd; ++cit);

		/* We will break if there were no characters found that matched to trim. */
		if(cit >= citEnd)
			break;
	}
	parser->str.begin = strIt;

	/* Trim end of string. */
	for(strIt = parser->str.end - 1; strIt > parser->str.begin; --strIt)
	{
		for(cit = chars; *cit != *strIt && cit != citEnd; ++cit);

		/* We will break if there were no characters found that matched to trim. */
		if(cit >= citEnd)
			break;
	}
	parser->str.end = strIt + 1;	/* strEnd should point to one past end of string. */

	if(parser->it.begin < parser->str.begin)
		StrParser_reset(parser);
	if(parser->it.end > parser->str.end)
		parser->it.end = parser->str.end;

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
		return(parser->str.end - parser->str.begin);
}
/* Returns the length of iterator being parsed. */
size_t StrParser_get_it_len(StrParser* parser)
{
	if(!parser || !parser->it.begin)
		return(0);
	else
		return(parser->it.end - parser->it.begin);
}
	/***********/
/********************/

/* Private Functions */

/*********************/

/* TODO: Move to C file. */
/* TODO: Add dynamic constructors. */
