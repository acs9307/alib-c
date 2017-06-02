#include "StrRef.h"

/* Simple reference object for referencing part of a string. */
typedef struct StrRef
{
	/* Beginning of the string. */
	const char* begin;
	/* One past the end of the string. */
	const char* end;
}StrRef;
