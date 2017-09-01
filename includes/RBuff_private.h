#ifndef ALIB_C_RBUFF_PRIVATE_IS_DEFINED
#define ALIB_C_RBUFF_PRIVATE_IS_DEFINED

#include "RBuff.h"

/* Rotational Buff */
struct RBuff
{
	void* buff;
	void* buffEnd;

	/* Pointer to the first byte written to the buffer. */
	void* itBegin;
	/* Pointer to the last byte written to the buffer.
	 * All data should be first written to this address. */
	void* itEnd;
	size_t count;

	RBuffFlag flags;
};

#endif
