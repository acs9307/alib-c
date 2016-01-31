#ifndef TIMER_PRIVATE_IS_DEFINED
#define TIMER_PRIVATE_IS_DEFINED

#include "Timer.h"

struct Timer
{
	/* Time start time (in millis). */
	uint64_t start_time;
	/* Time to run before ringing. */
	uint64_t run_time;
	
	/* !0 if the timer has rung and needs to 
	 * be reset. */
	char rang;
};

#endif