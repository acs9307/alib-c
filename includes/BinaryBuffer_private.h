#ifndef BINARY_BUFF_PRIVATE_IS_DEFINED
#define BINARY_BUFF_PRIVATE_IS_DEFINED

#include "BinaryBuffer.h"

/* Buffer object used to dynamically store data safely and efficiently.
 * Allocated memory exponentially grows.
 *
 * NOTES:
 * 		No function which modifies memory, either by reallocating or setting, will make
 * 			any modification if an error occurs.
 * 		This is not built with thread safety, to safely operate on the object, you must
 * 			build a wrapper object. */
struct BinaryBuffer
{
	/* The raw data buffer. */
	unsigned char* buff;
	/* The number of bytes stored in the buffer. */
	size_t len;
	/* The number number of bytes possible to store in the
	 * current buffer. */
	size_t capacity;

	/* The minimum capacity that the buffer must point to
	 * when it is not null. */
	size_t min_cap;
	/* The maximum number of bytes we can expand by on each
	 * expansion iteration. */
	size_t max_expand;
};

#endif
