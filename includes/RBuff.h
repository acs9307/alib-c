#ifndef ALIB_C_RBUFF_IS_DEFINED
#define ALIB_C_RBUFF_IS_DEFINED

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "alib_error.h"
#include "flags.h"

/* Binary flag. */
typedef enum RBuffFlag
{
	rbuffNoFlag = 0,
	/* If raised, the object will handle the lifecycle of the internal buffer. */
	rbuffOwnedBuff = 1,
	/* If raised, the object will not overwrite data in the buffer. */
	rbuffNoOverwrite = 2,
}RBuffFlag;

/* Rotational Buff */
typedef struct RBuff RBuff;

/* Public Functions */
	/* Getters */
/* Returns the total size of the buffer. */
size_t RBuff_get_size(RBuff* rbuff);
/* Returns the buffer count. */
size_t RBuff_get_count(RBuff* rbuff);
/* Returns the number of bytes available in the buffer. */
size_t RBuff_get_remaining(RBuff* rbuff);
	/***********/

/* Increments the end iterator by 'count'. */
void RBuff_increment_it(RBuff* rbuff, size_t count);

/* Copies the number of elements into the given buffer.
 *
 * Parameters:
 * 		rbuff: Pointer to the RBuff object.
 * 		buff: Pointer to the buffer to copy data into.
 * 		count: The number of bytes to copy.
 */
size_t RBuff_copy(RBuff* rbuff, void* buff, size_t count);

/* Same as RBuff_copy, but removes the elements that were copied. */
size_t RBuff_popoff(RBuff* rbuff, void* buff, size_t count);
/* Pops an item off the front of the buffer. */
int16_t RBuff_popoff_byte(RBuff* rbuff);

/* Pushes a buffer of data onto the RBuff. */
size_t RBuff_pushback(RBuff* rbuff, const void* buff, size_t count);
/* Pushes a byte onto the buffer. */
alib_error RBuff_pushback_byte(RBuff* rbuff, uint8_t byte);

/* Resets the RBuff to its initialized state. */
void RBuff_reset(RBuff* rbuff);
/********************/

/* Constructors */
alib_error RBuff_init(RBuff* rbuff, void* buff, size_t buffSize, RBuffFlag flags);

RBuff* newRBuff(size_t buffSize);
/****************/

/* Destructors */
void freeRBuff(RBuff* rbuff);
void delRBuff(RBuff** rbuff);
/***************/

#endif
