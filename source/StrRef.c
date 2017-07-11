#include "includes/StrRef_private.h"

/* Public Functions */
	/* Getters */
/* Returns the beginning of the referenced string. */
const char* StrRef_get_begin(const StrRef* ref)
{
	if(!ref)return(NULL);

	return(ref->begin);
}
/* Returns the end of the referenced string. */
const char* StrRef_get_end(const StrRef* ref)
{
	if(!ref)return(NULL);

	return(ref->end);
}
/* Returns the length of the referenced string.
 * Note: This may differ from 'strlen()' if we are referencing only a partial string. */
size_t StrRef_len(const StrRef* ref)
{
	if(!ref)return(0);

	return(ref->end - ref->begin);
}
	/***********/

	/* Constructors */
/* Initializes a StrRef object. */
alib_error StrRef_init(StrRef* ref, const char* begin, const char* end)
{
	if(!ref || !begin || !end || begin > end)return(ALIB_BAD_ARG);

	ref->begin = begin;
	ref->end = end;

	return(ALIB_OK);
}
/* Allocates a new StrRef object. */
StrRef* newStrRef(const char* begin, const char* end)
{
	if(!begin || !end || begin > end)return(NULL);

	StrRef* ref = malloc(sizeof(StrRef));
	if(!ref)goto f_return;

	if(StrRef_init(ref, begin, end))
		delStrRef(&ref);

f_return:
	return(ref);
}

/* Helper function that returns an initialized StrRef non-dynamic StrRef object. */
StrRef StrRef_from_string(const char* begin, const char* end)
{
	StrRef ref;
	StrRef_init(&ref, begin, end);
	return(ref);
}
	/****************/

	/* Destructors */
/* Frees a StrRef object.
 * 'alib_free_value' compliant. */
void freeStrRef(StrRef* ref)
{
	if(!ref)return;

	free(ref);
}
/* Fress a StrRef object and sets the point to NULL. */
void delStrRef(StrRef** ref)
{
	if(!ref)return;

	freeStrRef(*ref);
	*ref = NULL;
}
	/***************/
/********************/
