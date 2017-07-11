#ifndef ALIB_C_STR_REF_IS_DEFINED
#define ALIB_C_STR_REF_IS_DEFINED

#include "alib_error.h"

#include <stdio.h>
#include <stdlib.h>

/* Simple reference object for referencing part of a string. */
typedef struct StrRef StrRef;

/* Public Functions */
	/* Getters */
/* Returns the beginning of the referenced string. */
const char* StrRef_get_begin(const StrRef* ref);
/* Returns the end of the referenced string. */
const char* StrRef_get_end(const StrRef* ref);
/* Returns the length of the referenced string.
 * Note: This may differ from 'strlen()' if we are referencing only a partial string. */
size_t StrRef_len(const StrRef* ref);
	/***********/

	/* Constructors */
/* Initializes a StrRef object. */
alib_error StrRef_init(StrRef* ref, const char* begin, const char* end);
/* Allocates a new StrRef object. */
StrRef* newStrRef(const char* begin, const char* end);

/* Helper function that returns an initialized StrRef non-dynamic StrRef object. */
StrRef StrRef_from_string(const char* begin, const char* end);
	/****************/

	/* Destructors */
/* Frees a StrRef object.
 * 'alib_free_value' compliant. */
void freeStrRef(StrRef* ref);
/* Fress a StrRef object and sets the point to NULL. */
void delStrRef(StrRef** ref);
	/***************/
/********************/

#endif
