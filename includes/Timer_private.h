#ifndef TIMER_PRIVATE_IS_DEFINED
#define TIMER_PRIVATE_IS_DEFINED

#include "Timer.h"
#include "TimerBase_private.h"

/* Basic timer object.  Stores a start time and a run time.
 * When a 'Timer_check()' call is made on the object, the time difference
 * is checked which will tell the caller if the timer has rung or not.
 *
 * The Timer object has memory so that once it has rung, it must be reset
 * before it can be reused.  This is done simply by calling 'Timer_begin()'.
 *
 * This uses all 64 bit integers for storing time.  If a timer is only needed
 * for a short period of time try using 'TimerLight', it uses 'size_t' instead
 * of 'uint64_t'.
 *
 * Inherits from 'TimerBase' (Private). */
struct Timer
{
	/* Base struct. */
	TimerBase base;

	/* Time start time. */
	uint64_t start_time;
	/* Time to run before ringing. */
	uint64_t run_time;
};

#endif
