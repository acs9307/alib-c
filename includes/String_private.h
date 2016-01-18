#ifndef STRING_PRIVATE_IS_DEFINED
#define STRING_PRIVATE_IS_DEFINED

#include "String.h"
#include "BinaryBuffer_private.h"

/* Simple object inherited from BinaryBuffer.  Handles basic string operations
 * and ensures that there is always a null terminator at the end of the string.
 *
 * All buffer allocation is handled automatically.
 *
 * Note:
 * 		Though String inherits BinaryBuffer, it is not suggested that you be
 * 			very careful about using BinaryBuffer methods on this object as
 * 			it may corrupt the internal c-string because BinaryBuffer does
 * 			not worry about a null terminator.  */
struct String
{
	BinaryBuffer base;
};

#endif
