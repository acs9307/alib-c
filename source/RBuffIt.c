#include "RBuffIt_private.h"

/* Private Functions */
/* Checks for errors with the iterator.  This may occur if the RBuff is updated in the middle of
 * an operation.
 *
 * Returns: True if a recovery took place. */
static uint8_t _recover(RBuffIt* it)
{
	uint8_t rval = 0;
	if(!it)
		return(0);

	if(it->rbuff->itBegin > it->rbuff->itEnd)
	{
		if(it->it < it->rbuff->itBegin && it->it > it->rbuff->itEnd)
		{
			it->it = NULL;
			rval = 1;
		}
	}
	else
	{
		if(it->it < it->rbuff->itBegin || it->it > it->rbuff->itEnd)
		{
			it->it = NULL;
			rval = 1;
		}
	}

	return(rval);
}
/*********************/

/* Public Functions */
/* Returns the index of the iterator in the RBuff. */
int RBuffIt_get_index(RBuffIt* it)
{
	if(!it || !it->it)
		return(-1);

	size_t index = 0;

	if(it->it < it->rbuff->itBegin)
	{
		index += it->rbuff->buffEnd - it->rbuff->itBegin;
		index += it->it - it->rbuff->buff;
	}
	else
		index = it->it - it->rbuff->itBegin;

	return(index);
}

/* Moves the iterator to the next item in the RBuff.
 *
 * Returns:
 * 		true: If the iterator can continue moving.
 * 		false: If the iterator has hit the end of the buffer or when an error occurs. */
uint8_t RBuffIt_next(RBuffIt* it)
{
	if(!it)
		return(0);

	if(!it->it)
	{
		if(!it->rbuff->count)
			return(0);
		else
			it->it = it->rbuff->itBegin;
	}
	else
	{
		++it->it;

		if(it->it >= it->rbuff->buffEnd)
			it->it = it->rbuff->buff;

		if(it->it == it->rbuff->itEnd)
		{
			it->it = NULL;
			return(0);
		}
	}

	return(1);
}
/* Moves the iterator to the previous item in the RBuff. */
uint8_t RBuffIt_prev(RBuffIt* it)
{
	if(!it)return(0);

	if(!it->it)
	{
		if(it->rbuff->count == 0)
			return(0);

		it->it = it->rbuff->itEnd;
	}
	else if(it->it == it->rbuff->itBegin)
	{
		it->it = NULL;
		return(0);
	}
	else
	{
		--it->it;
		if(it->it < it->rbuff->buff)
			it->it = it->rbuff->buffEnd - 1;
	}

	return(1);
}

/* Resets the iterator pointer to point to the beginning of the RBuff. */
void RBuffIt_reset(RBuffIt* it)
{
	if(!it)return;

	it->it = NULL;
}
/********************/

/* Constructors */
/* Initializes an RBuffIt.
 *
 * Parameters:
 * 		it: The iterator to initialize.
 * 		rbuff: The buffer to iterate through.  Note: This buffer is not owned by the object. */
alib_error RBuffIt_init(RBuffIt* it, RBuff* rbuff)
{
	if(!it || !rbuff)return(ALIB_BAD_ARG);

	it->rbuff = rbuff;
	it->it = NULL;

	return(ALIB_OK);
}
/* Generates a new RBuffIt object. */
RBuffIt* newRBuffIt(RBuff* rbuff)
{
	if(!rbuff)
		return(NULL);

	RBuffIt* it = (RBuffIt*)malloc(sizeof(RBuffIt));
	if(!it)
		goto f_return;

	if(RBuffIt_init(it, rbuff))
		delRBuffIt(&it);

f_return:

	return(it);
}
/****************/

/* Destructors */
/* Frees an RBuffIt object. */
void freeRBuffIt(RBuffIt* it)
{
	if(it)
	{
		free(it);
	}
}
/* Deletes an RBuffIt object. */
void delRBuffIt(RBuffIt** it)
{
	if(it)
	{
		freeRBuffIt(*it);
		*it = NULL;
	}
}
/***************/
