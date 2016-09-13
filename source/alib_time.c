#include "alib_time.h"

#ifdef __linux__
/* Attempts to fix the values in a timespec structure so that there
 * are no combination of negative and positive values within a
 * single timespec.  Also ensures that 'tv_nsec' does not overflow
 * past NANOS_PER_SECOND.
 *
 * Assumes 't' is not null. */
void timespec_fix_values_fast(struct timespec* t)
{
	/* Check for 'tv_nsec' overflow or underflow. */
	if(t->tv_nsec > NANOS_PER_SECOND || t->tv_nsec < -NANOS_PER_SECOND)
	{
		t->tv_sec += t->tv_nsec / NANOS_PER_SECOND;
		t->tv_nsec = t->tv_nsec % NANOS_PER_SECOND;
	}

	/* See if we can make them all the same sign. */
	if(t->tv_nsec < 0 && t->tv_sec > 0)
	{
		--t->tv_sec;
		t->tv_nsec += NANOS_PER_SECOND;
	}
	else if(t->tv_nsec > 0 && t->tv_sec < 0)
	{
		++t->tv_sec;
		t->tv_nsec -= NANOS_PER_SECOND;
	}
}
/* Attempts to fix the values in a timespec structure so that there
 * are no combination of negative and positive values within a
 * single timespec.  Also ensures that 'tv_nsec' does not overflow
 * past NANOS_PER_SECOND.
 *
 * Differs from 'timespec_fix_values_fast()' in that it checks for any
 * errors that may occur. */
alib_error timespec_fix_values(struct timespec* t)
{
	timespec_fix_values_fast(t);

	if((t->tv_sec < 0 && t->tv_nsec > 0) ||
			(t->tv_sec > 0 && t->tv_nsec < 0))
		return(ALIB_OBJ_CORRUPTION);

	return(ALIB_OK);
}

/* Compares two timespec structures.  This should be used
 * instead of 'timespec_cmp()' when comparison is time critical and
 * you know that the parameters do not need to be fixed.
 *
 * Returns:
 * 		1: 'a' is greater than 'b' (i.e. 'a' came after 'b').
 * 		0: 'a' and 'b' are equal.
 * 		-1: 'a' is less than 'b' (i.e. 'b' came after 'a').
 *
 * Assumes 'a' and 'b' are not null and that both have been
 * 'fixed' (see 'timespec_fix()'). */
char timespec_cmp_fast(const struct timespec* a, const struct timespec* b)
{
	if(a->tv_sec > b->tv_sec)
		return(1);
	else if(a->tv_sec == b->tv_sec)
	{
		if(a->tv_nsec > b->tv_nsec)
			return(1);
		else if(a->tv_nsec == b->tv_nsec)
			return(0);
		else
			return(-1);
	}
	else
		return(-1);
}
/* Same as timespec_cmp_fast, except that the values are first fixed before
 * comparing.  This is good for when the comparison is not time critical and
 * you are unsure if the values must be fixed.
 *
 * Returns:
 * 		1: 'a' is greater than 'b' (i.e. 'a' came after 'b').
 * 		0: 'a' and 'b' are equal.
 * 		-1: 'a' is less than 'b' (i.e. 'b' came after 'a'). */
char timespec_cmp(struct timespec a, struct timespec b)
{
	timespec_fix_values(&a);
	timespec_fix_values(&b);

	return(timespec_cmp_fast(&a, &b));
}
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
		struct timespec* diff)
{
	diff->tv_sec = a->tv_sec - b->tv_sec;
	diff->tv_nsec = a->tv_nsec - b->tv_nsec;
	timespec_fix_values_fast(diff);
	return(diff);
}
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
		struct timespec* sum)
{
	sum->tv_sec = a->tv_sec + b->tv_sec;
	sum->tv_nsec = a->tv_nsec + b->tv_nsec;
	timespec_fix_values_fast(sum);
	return(sum);
}

/* Initializes a timespec struct with the given values and ensures the values
 * are fixed by calling 'timespec_fix_values()' on the struct.
 *
 * Returns the value returned from 'timespec_fix_values()'. */
alib_error timespec_init(struct timespec* a, long sec, long nsec)
{
	*a = (struct timespec){sec, nsec};
	return(timespec_fix_values(a));
}

#endif