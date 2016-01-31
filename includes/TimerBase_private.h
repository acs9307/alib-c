#ifndef TIMER_BASE_PRIVATE_IS_DEFINED
#define TIMER_BASE_PRIVATE_IS_DEFINED

#include "TimerBase.h"

/* Base structure for Timer objects.  Stores the state of the
 * timer and the magnitude of the times stored. */
struct TimerBase
{
	/* !0 if the timer has rung and needs to
	 * be reset. */
	uint8_t rang;

	/* The magnitude that the timer uses. */
	MagnitudeTime magnitude;

	struct timespec time;
};

#endif
