#ifndef TIMER_BASE_IS_DEFINED
#define TIMER_BASE_IS_DEFINED

#include <time.h>
#include <inttypes.h>
#include <stdlib.h>

/* Included only because Android NDK does not have this in 'time.h'.
 * Thanks google... */
#ifndef __timespec_defined
#define __timespec_defined 1
struct timespec
{
	long tv_sec;		/* Seconds.  */
	long tv_nsec;	/* Nanoseconds.  */
};
#endif
#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC 1
#endif

typedef enum MagnitudeTime
{
	SECONDS = 0,
	MILLIS = -1,
	MICROS = -2,
	NANOS = -3,
	PICOS = -4
}MagnitudeTime;

/* Base structure for Timer objects.  Stores the state of the
 * timer and the magnitude of the times stored. */
typedef struct TimerBase TimerBase;

/*******Destructors*******/
/* Frees a timer object. */
void freeTimerBase(TimerBase* t);
/* Deletes a timer object. */
void delTimerBase(TimerBase** t);
/*************************/

#endif
