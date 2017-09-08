#ifndef ALIB_C_RBUFFIT_IS_DEFINED
#define ALIB_C_RBUFFIT_IS_DEFINED

#include "RBuff.h"
#include "alib_error.h"

/* Iterator object for RBuffs. */
typedef struct RBuffIt RBuffIt;

/* Public Functions */
/* Returns the index of the iterator in the RBuff. */
int RBuffIt_get_index(RBuffIt* it);

/* Moves the iterator to the next item in the RBuff.
 *
 * Returns:
 * 		true: If the iterator can continue moving.
 * 		false: If the iterator has hit the end of the buffer or when an error occurs. */
uint8_t RBuffIt_next(RBuffIt* it);
/* Moves the iterator to the previous item in the RBuff. */
uint8_t RBuffIt_prev(RBuffIt* it);

/* Resets the iterator pointer to point to the beginning of the RBuff. */
void RBuffIt_reset(RBuffIt* it);
/********************/

/* Constructors */
/* Initializes an RBuffIt.
 *
 * Parameters:
 * 		it: The iterator to initialize.
 * 		rbuff: The buffer to iterate through.  Note: This buffer is not owned by the object. */
alib_error RBuffIt_init(RBuffIt* it, RBuff* rbuff);
/* Generates a new RBuffIt object. */
RBuffIt* newRBuffIt(RBuff* rbuff);
/****************/

/* Destructors */
/* Frees an RBuffIt object. */
void freeRBuffIt(RBuffIt* it);
/* Deletes an RBuffIt object. */
void delRBuffIt(RBuffIt** it);
/***************/

#endif
