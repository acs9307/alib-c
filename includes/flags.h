#ifndef FLAGS_IS_DEFINED
#define FLAGS_IS_DEFINED

#include <stdio.h>

/* A binary flag type.  Flags should be able to be ANDed and ORed together. */
typedef size_t flag_pole;

typedef enum object_flag_pole
{
	/* Set when the thread has had 'pthread_create()' called on
	 * it at least once.  If this is not set, pthread_join is not
	 * safe to call on the related thread. */
	THREAD_CREATED = 1,
	/* Set when the thread is running.  Should be unset when
	 * the thread returns. */
	THREAD_IS_RUNNING = 2,
	/* When set, the thread should attempt to return on its next
	 * iteration. */
	THREAD_STOP = 4,
	/* When set, requests that the thread never return unless called
	 * to stop. */
	THREAD_KEEP_ALIVE = 8,

	/* When set, the object is currently being deleted. */
	OBJECT_DELETE_STATE = 16,
	/* When set, the object is currently running a callback function. */
	OBJECT_CALLBACK_STATE = 32,
}object_flag_pole;

/* Raises a binary flag, if the 'flag' was already raised, no changes
 * will occur to 'flag_pole'.
 *
 * Parameters:
 * 		flag_pole (flag_pole*): The integer that stores all the related flags.
 * 			If NULL, then 0 will be returned.
 * 		flag (size_t): The flag to raise.  This value can be a combination of AND, ORed,
 * 			and XORed values.
 *
 * Returns:
 * 		The value of the flag pole after modification. */
#define flag_raise(flag_pole, flag) (*(flag_pole) |= flag)
/* Lowers (or removes) a binary flag. If the 'flag' was not raised, no changes
 * will occur to 'flag_pole'.
 *
 * Parameters:
 * 		flag_pole (flag_pole*): Integer that stores all the related flags.
 * 			If NULL, then 0 will be returned.
 * 		flag (size_t): The flag to raise.  This value can be a combination of AND, ORed,
 * 			and XORed values.
 *
 * Returns:
 * 		The value of the flag pole after modification. */
#define flag_lower(flag_pole, flag) (*(flag_pole) &= ~flag)

/* Removes any and all flags from the flag pole.
 *
 * Parameters:
 * 		flag_pole (flag_pole*): The flag pole to clear.
 *
 * Returns:
 * 		The value of the flag pole. */
#define flag_clear(flag_pole) (*(flag_pole) = 0)

/* Initalizes a flag pole. */
#define FLAG_INIT 0

#endif
