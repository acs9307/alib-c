#ifndef ALIB_C_STR_REF_PRIVATE_IS_DEFINED
#define ALIB_C_STR_REF_PRIVATE_IS_DEFINED

#include "StrRef.h"

/* Simple reference object for referencing part of a string. */
struct StrRef
{
	/* Beginning of the string. */
	const char* begin;
	/* One past the end of the string. */
	const char* end;
};

#endif
