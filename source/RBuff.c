#include "includes/RBuff_private.h"

/* Private Functions */
	/* Getters */
/* Returns the number of bytes from the end it to the end of the buffer.
 * This will stop at either the end of the actual buffer, or
 * stop at the beginning iterator, whichever is shorter. */
static size_t _endItToBuffEnd(RBuff* rbuff)
{
	if(!rbuff)
		return(0);
	else
	{
		if(rbuff->itEnd < rbuff->itBegin)
			return(rbuff->itBegin - rbuff->itEnd);
		else
			return(rbuff->buffEnd - rbuff->itEnd);
	}
}
/* Returns the number of bytes from the beginning it to the end of the buffer.
 * This will stop at either the end of the actual buffer, or
 * stop at the end iterator, whichever is shorter. */
static size_t _beginItToBuffEnd(RBuff* rbuff)
{
	if(!rbuff)
		return(0);
	else
	{
		if(rbuff->itBegin < rbuff->itEnd)
			return(rbuff->itEnd - rbuff->itBegin);
		else
			return(rbuff->buffEnd - rbuff->itBegin);
	}
}
	/***********/

/* Increments the iterator. */
static void _incrementIt(RBuff* rbuff, void** it, size_t count)
{
	if(!rbuff || !it || !*it)return;

	size_t buffSize = RBuff_get_size(rbuff);

	/* If we are incrementing more than the size of the buffer,
	 * then we do a modulus with the buff size so that we
	 * don't loop back to the same position multiple times. */
	if(count > buffSize)
		count = count % buffSize;

	/* Increment by the count. */
	*it += count;

	/* If we went past the end of the buffer, then simply subtract the buffer size
	 * and it will place the pointer in the correct position. */
	if(*it >= rbuff->buffEnd)
		*it -= buffSize;
}
/*********************/

/* Public Functions */
	/* Getters */
/* Returns the total size of the buffer. */
size_t RBuff_get_size(RBuff* rbuff)
{
	if(!rbuff)
		return(0);
	else
		return(rbuff->buffEnd - rbuff->buff);
}
/* Returns the buffer count. */
size_t RBuff_get_count(RBuff* rbuff)
{
#if 0
	if(!rbuff)
		return(0);

	size_t count = 0;

	if(rbuff->itEnd < rbuff->itBegin)
	{
		count = rbuff->buffEnd - rbuff->itBegin;
		count += rbuff->itEnd - rbuff->buff;
	}
	else
		count = rbuff->itEnd - rbuff->itBegin;

	return(count);
#else
	return(rbuff->count);
#endif
}
/* Returns the number of bytes available in the buffer. */
size_t RBuff_get_remaining(RBuff* rbuff)
{
	if(!rbuff)return(0);

	return(RBuff_get_size(rbuff) - RBuff_get_count(rbuff));
}
	/***********/

/* Increments the end iterator by 'count'. */
void RBuff_increment_it(RBuff* rbuff, size_t count)
{
	if(!rbuff || !count)return;

	size_t buffSize = RBuff_get_size(rbuff);

	if(rbuff->count + count > buffSize)
	{
		_incrementIt(rbuff, &rbuff->itBegin, count);
		rbuff->count = buffSize;
	}
	else
		rbuff->count += count;

	/* Increment end afterwards as it may be pointing to the*/
	_incrementIt(rbuff, &rbuff->itEnd, count);
}

/* Copies the number of elements into the given buffer.
 *
 * Parameters:
 * 		rbuff: Pointer to the RBuff object.
 * 		buff: Pointer to the buffer to copy data into.
 * 		count: The number of bytes to copy.
 */
size_t RBuff_copy(RBuff* rbuff, void* buff, size_t count)
{
	if(!rbuff || !count)return(0);

	size_t totalCount = 0;
	size_t copyCount = 0;
	size_t rbuffCount = RBuff_get_count(rbuff);
	void* itBegin = rbuff->itBegin;

	/* Can't copy more than we have. */
	if(count > rbuffCount)
		count = rbuffCount;

	/* Copy first half. */
	copyCount = _beginItToBuffEnd(rbuff);
	if(copyCount < count)
	{
		if(buff)
		{
			memcpy(buff, itBegin, copyCount);
			buff += copyCount;
		}
		itBegin = rbuff->buff;
		count -= copyCount;
		totalCount += copyCount;
	}

	/* Copy second half. */
	if(buff)
		memcpy(buff, itBegin, count);
	totalCount += count;

	return(totalCount);
}

/* Same as RBuff_copy, but removes the elements that were copied. */
size_t RBuff_popoff(RBuff* rbuff, void* buff, size_t count)
{
	size_t rval = RBuff_copy(rbuff, buff, count);
	_incrementIt(rbuff, &rbuff->itBegin, rval);
	rbuff->count -= rval;
	return(rval);
}
/* Pops an item off the front of the buffer. */
int16_t RBuff_popoff_byte(RBuff* rbuff)
{
	if(!rbuff || !RBuff_get_count(rbuff))
		return(ALIB_BAD_ARG);

	uint8_t rval = *(uint8_t*)rbuff->itBegin;
	_incrementIt(rbuff, &rbuff->itBegin, 1);
	--rbuff->count;
	return(rval);
}

/* Pushes a buffer of data onto the RBuff. */
size_t RBuff_pushback(RBuff* rbuff, const void* buff, size_t count)
{
	if(!rbuff || !buff || !count)return(0);

	int32_t totalCount = 0;
	size_t copyCount;
	const size_t buffSize = RBuff_get_size(rbuff);
	size_t buffCount = RBuff_get_count(rbuff);

	/* If the size exceeds the buffer size, then we must change the number of
	 * bytes we copy. */
	if(count > buffSize)
	{
		buff += count - buffSize;
		count = buffSize;
	}

	/* Pop items off if there are too many. */
	if(count + buffCount > buffSize)
	{
		buffCount -= RBuff_popoff(rbuff, NULL, (count + buffCount) - buffSize);
	}

	/* Copy first half */
	copyCount = _endItToBuffEnd(rbuff);
	if(count > copyCount)
	{
		memcpy(rbuff->itEnd, buff, copyCount);
		rbuff->itEnd = rbuff->buff;
		count -= copyCount;
		totalCount += copyCount;
		buff += copyCount;
		copyCount = count;
	}

	/* Copy second half */
	if(copyCount > count)
		copyCount = count;

	memcpy(rbuff->itEnd, buff, copyCount);
	rbuff->itEnd += copyCount;
	totalCount += copyCount;

	rbuff->count += totalCount;
	if(rbuff->count > buffSize)
		rbuff->count = buffSize;

	return(totalCount);
}
/* Pushes a byte onto the buffer. */
alib_error RBuff_pushback_byte(RBuff* rbuff, uint8_t byte)
{
	if(!rbuff)return(ALIB_BAD_ARG);

	*(uint8_t*)rbuff->itEnd = byte;
	RBuff_increment_it(rbuff, 1);
	return(ALIB_OK);
}

/* Resets the RBuff to its initialized state. */
void RBuff_reset(RBuff* rbuff)
{
	if(!rbuff)return;

	RBuff_init(rbuff, rbuff->buff, RBuff_get_size(rbuff), rbuff->flags);
}
/********************/

/* Constructors */
alib_error RBuff_init(RBuff* rbuff, void* buff, size_t buffSize, RBuffFlag flags)
{
	if(!rbuff || !buff || !buffSize)return(ALIB_BAD_ARG);

	rbuff->buff = buff;
	rbuff->buffEnd = buff + buffSize;

	rbuff->itBegin = buff;
	rbuff->itEnd = buff;
	rbuff->count = 0;
	rbuff->flags = flags;

	return(ALIB_OK);
}

RBuff* newRBuff(size_t buffSize)
{
	if(!buffSize)
		return(NULL);

	void* buff = NULL;
	RBuff* rbuff = NULL;

	buff = malloc(buffSize);
	rbuff = (RBuff*)malloc(sizeof(RBuff));
	if(!buff || !rbuff)
	{
		if(buff)
			free(buff);
		if(rbuff)
			free(rbuff);

		buff = rbuff = NULL;
		goto f_return;
	}

	if(RBuff_init(rbuff, buff, buffSize, rbuffOwnedBuff))
		delRBuff(&rbuff);

f_return:
	return(rbuff);
}
/****************/

/* Destructors */
void freeRBuff(RBuff* rbuff)
{
	if(!rbuff)return;

	if((rbuff->flags & rbuffOwnedBuff) && rbuff->buff)
		free(rbuff->buff);
	free(rbuff);
}
void delRBuff(RBuff** rbuff)
{
	if(!rbuff)return;

	freeRBuff(*rbuff);
	*rbuff = NULL;
}
/***************/
