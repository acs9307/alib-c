#ifndef TIMER_PRIVATE_IS_DEFINED
#define TIMER_PRIVATE_IS_DEFINED

#include "Timer.h"

/* Basic timer object.  Stores a start time and a run time.
 * When a 'Timer_check()' call is made on the object, the time difference
 * is checked which will tell the caller if the timer has rung or not.
 *
 * The Timer object has memory so that once it has rung, it must be reset
 * before it can be reused.  This is done simply by calling 'Timer_begin()'. */
struct Timer
{
	uint8_t rang;
	struct timespec run_time;
	struct timespec end_time;
};

#endif
