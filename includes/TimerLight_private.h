#ifndef TIMER_LIGHT_PRIVATE_IS_DEFINED
#define TIMER_LIGHT_PRIVATE_IS_DEFINED

#include "TimerLight.h"
#include "TimerBase_private.h"

/* Basic timer object.  Stores a start time and a run time.
 * When a 'TimerLight_check()' call is made on the object, the time difference
 * is checked which will tell the caller if the timer has rung or not.
 *
 * The Timer object has memory so that once it has rung, it must be reset
 * before it can be reused.  This is done simply by calling 'TimerLight_begin()'.
 *
 * This works exactly the same as Timer, however it uses 'size_t' members for
 * storing time rather than 'uint64_t' like Timer.  This is only advantageous
 * if you need extremely high speed timer calculations or you must minimize
 * your memory footprint.
 *
 * Inherits from 'TimerBase' (Private). */
struct TimerLight
{
	/* Base struct. */
	TimerBase base;

	/* Time start time (in millis). */
	size_t start_time;
	/* Time to run before ringing. */
	size_t run_time;
};

#endif
