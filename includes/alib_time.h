#ifndef ALIB_TIME_IS_DEFINED
#define ALIB_TIME_IS_DEFINED

#include <stdlib.h>
#include <time.h>

#include "alib_error.h"

#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC 1
#endif

/*******Conversion Defines*******/
	/* To Seconds */
#ifndef MILLIS_PER_SECOND
#define MILLIS_PER_SECOND (1000)
#endif
#ifndef MICROS_PER_SECOND
#define MICROS_PER_SECOND (1000 * MILLIS_PER_SECOND)
#endif
#ifndef NANOS_PER_SECOND
#define NANOS_PER_SECOND (1000 * MICROS_PER_SECOND)
#endif
#ifndef PICOS_PER_SECOND
#define PICOS_PER_SECOND \
	(uint64_t)((uint64_t)1000 * (uint64_t)NANOS_PER_SECOND)
#endif
	/**************/

	/* To Millis */
#ifndef MICROS_PER_MILLIS
#define MICROS_PER_MILLIS (1000)
#endif
#ifndef NANOS_PER_MILLIS
#define NANOS_PER_MILLIS (1000 * MICROS_PER_MILLIS)
#endif
#ifndef PICOS_PER_MILLIS
#define PICOS_PER_MILLIS (1000 * NANOS_PER_MILLIS)
#endif
	/*************/

	/* To Micros */
#ifndef NANOS_PER_MICROS
#define NANOS_PER_MICROS (1000)
#endif
#ifndef PICOS_PER_MICROS
#define PICOS_PER_MICROS (1000 * NANOS_PER_MICROS)
#endif
	/*************/

	/* To Nanos */
#ifndef PICOS_PER_NANOS
#define PICOS_PER_NANOS (1000)
#endif
	/************/
/********************************/

/*******From Seconds*******/
#ifndef SECONDS_TO_MILLIS
#define SECONDS_TO_MILLIS(sec) (sec * MILLIS_PER_SECOND)
#endif
#ifndef SECONDS_TO_MICROS
#define SECONDS_TO_MICROS(sec) (sec * MICROS_PER_SECOND)
#endif
#ifndef SECONDS_TO_NANOS
#define SECONDS_TO_NANOS(sec) (sec * NANOS_PER_SECOND)
#endif
#ifndef SECONDS_TO_PICOS
#define SECONDS_TO_PICOS(sec) (sec * PICOS_PER_SECOND)
#endif
/**************************/

/*******From Millis*******/
#ifndef MILLIS_TO_SECONDS
#define MILLIS_TO_SECONDS(millis) (millis / MILLIS_PER_SECOND)
#endif
#ifndef MILLIS_TO_MICROS
#define MILLIS_TO_MICROS(millis) (millis * MICROS_PER_MILLIS)
#endif
#ifndef MILLIS_TO_NANOS
#define MILLIS_TO_NANOS(millis) (millis * NANOS_PER_MILLIS)
#endif
#ifndef MILLIS_TO_PICOS
#define MILLIS_TO_PICOS(millis) (millis * PICOS_PER_MILLIS)
#endif
/*************************/

/*******From Micros*******/
#ifndef MICROS_TO_SECONDS
#define MICROS_TO_SECONDS(micros) (micros / MICROS_PER_SECOND)
#endif
#ifndef MICROS_TO_MILLIS
#define MICROS_TO_MILLIS(micros) (micros / MICROS_PER_MILLIS)
#endif
#ifndef MICROS_TO_NANOS
#define MICROS_TO_NANOS(micros) (micros * NANOS_PER_MICROS)
#endif
#ifndef MICROS_TO_PICOS
#define MICROS_TO_PICOS(micros) (micros * PICOS_PER_MICROS)
#endif
/*************************/

/*******From Nanos*******/
#ifndef NANOS_TO_SECONDS
#define NANOS_TO_SECONDS(nanos) (nanos / NANOS_PER_SECOND)
#endif
#ifndef NANOS_TO_MILLIS
#define NANOS_TO_MILLIS(nanos) (nanos / NANOS_PER_MILLIS)
#endif
#ifndef NANOS_TO_MICROS
#define NANOS_TO_MICROS(nanos) (nanos / NANOS_PER_MICROS)
#endif
#ifndef NANOS_TO_PICOS
#define NANOS_TO_PICOS(nanos) (nanos * PICOS_PER_NANOS)
#endif
/************************/

/*******From Pico*******/
#ifndef PICOS_TO_SECONDS
#define PICOS_TO_SECONDS(picos) (picos / PICOS_PER_SECOND)
#endif
#ifndef PICOS_TO_MILLIS
#define PICOS_TO_MILLIS(picos) (picos / PICOS_PER_MILLIS)
#endif
#ifndef PICOS_TO_MICROS
#define PICOS_TO_MICROS(picos) (picos / PICOS_PER_MICROS)
#endif
#ifndef PICOS_TO_NANOS
#define PICOS_TO_NANOS(picos) (picos / PICOS_PER_NANOS)
#endif
/***********************/

/*******Timespec Functions*******/
#include "alib_time.h"

/* Attempts to fix the values in a timespec structure so that there
 * are no combination of negative and positive values within a
 * single timespec.  Also ensures that 'tv_nsec' does not overflow
 * past NANOS_PER_SECOND.
 *
 * Assumes 't' is not null. */
void timespec_fix_values_fast(struct timespec* t);
/* Attempts to fix the values in a timespec structure so that there
 * are no combination of negative and positive values within a
 * single timespec.  Also ensures that 'tv_nsec' does not overflow
 * past NANOS_PER_SECOND.
 *
 * Differs from 'timespec_fix_values_fast()' in that it checks for any
 * errors that may occur. */
alib_error timespec_fix_values(struct timespec* t);

/* Compares two timespec structures.  This should be used
 * instead of 'timespec_cmp()' when comparison is time critical and
 * you know that the parameters do not need to be fixed.
 *
 * Returns:
 * 		1: 'a' is greater than 'b' (i.e. 'a' came after 'b').
 * 		0: 'a' and 'b' are equal.
 * 		-1: 'b' is less than 'a' (i.e. 'b' came after 'a').
 *
 * Assumes 'a' and 'b' are not null and that both have been
 * 'fixed' (see 'timespec_fix()'). */
char timespec_cmp_fast(const struct timespec* a, const struct timespec* b);
/* Same as timespec_cmp_fast, except that the values are first fixed before
 * comparing.  This is good for when the comparison is not time critical and
 * you are unsure if the values must be fixed.
 *
 * Returns:
 * 		1: 'a' is greater than 'b' (i.e. 'a' came after 'b').
 * 		0: 'a' and 'b' are equal.
 * 		-1: 'b' is less than 'a' (i.e. 'b' came after 'a'). */
char timespec_cmp(struct timespec a, struct timespec b);
/* Finds the difference between two timespec structs and places the difference in
 * the 'diff' struct.  All parameters are required.
 *
 * Algorithm: a - b = diff
 *
 * Parameters: Behavior is undefined if a null parameter is passed.
 * 		a: The minuend.
 * 		b: The subtrahend.
 * 		diff: The difference.
 *
 * Returns 'diff'.
 *
 * Assumes there are no null values passed. */
struct timespec* timespec_subtract(struct timespec* a, struct timespec* b,
		struct timespec* diff);
/* Adds two timespecs together.
 *
 * Algorithm: a + b = sum
 *
 * Parameters: Behavior is undefined if a null parameter is passed.
 * 		a: The first value to add.
 * 		b: The second value to add.
 * 		sum: The sum of 'a' and 'b'.
 *
 * Returns 'sum'.
 *
 * Assumes there are no null values passed. */
struct timespec* timespec_add(struct timespec* a, struct timespec* b,
		struct timespec* sum);

/* Initializes a timespec struct with the given values and ensures the values
 * are fixed by calling 'timespec_fix_values()' on the struct.
 *
 * Returns the value returned from 'timespec_fix_values()'. */
alib_error timespec_init(struct timespec* a, long sec, long nsec);
/********************************/

#endif
