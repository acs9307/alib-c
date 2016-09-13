#ifndef TIMER_IS_DEFINED
#define TIMER_IS_DEFINED

#include <time.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "alib_error.h"
#include "alib_time.h"

/* Basic timer object.  Stores a start time and a run time.
 * When a 'Timer_check()' call is made on the object, the time difference
 * is checked which will tell the caller if the timer has rung or not.
 *
 * The Timer object has memory so that once it has rung, it must be reset
 * before it can be reused.  This is done simply by calling 'Timer_begin()'. */
typedef struct Timer Timer;


/*******Public Functions*******/
/* Resets the timer and sets the end time of the timer. */
void Timer_begin(Timer* t);
/* Same as 'Timer_begin()' except it allows you to set the end time
 * to an earlier time. This is useful when you need to ensure the timer
 * rings are caught with more precision as 'Timer_get_time_status()' may take
 * several micro seconds.
 *
 * Parameters:
 * 		t: The object to modify.
 * 		interval: The time interval to subtract from the current time. */
void Timer_begin_before(Timer* t, struct timespec* interval);
/* Checks to see if the timer has rung.  Once a timer has rung, it will
 * stay in the rung state until it is reset by 'Timer_reset()'.
 *
 * Returns:
 * 		>0: Timer has rung.
 * 		 0: Timer has not rung yet.
 * 		<0: alib_error */
int Timer_check(Timer* t);

	/* Getters */
/* Returns the time that the timer was started.
 *
 * Assumes 't' is not null. */
const struct timespec* Timer_get_end_time(Timer* t);
/* Returns the real time version of the end time.
 *
 * If an error occurs, function returns a zeroed out timespec. */
struct timespec Timer_get_end_time_real_time(Timer* t);
/* Returns the time to wait before ringing the timer.
 *
 * Assumes 't' is not null. */
const struct timespec* Timer_get_run_time(Timer* t);
/* Returns the last checked rung state of the timer.
 *
 * Assumes 't' is not null. */
char Timer_get_rung_state(Timer* t);

/* Returns both the remaining time and the overflow time for the timer.
 * If there is no time remaining, 'rTime' will be set to 0.  If there is
 * time remaining, 'oTime' will be set to 0.
 *
 * Parameters:
 * 		t: The timer to check.
 * 		rTime: (OPTIONAL) The remaining time.
 * 		oTime: (OPTIONAL) The overflow time.
 *
 * Returns:
 * 		alib_error: If ALIB_UNKNOWN_ERR is returned, then something occurred where
 * 			the calculated time value came out with */
alib_error Timer_get_time_status(Timer* t, struct timespec* rTime,
		struct timespec* oTime);
	/***********/

	/* Setters */
/* Sets the run time of the timer.
 *
 * Assumes 't' is not null. */
void Timer_set_run_time(Timer* t, size_t runtime_sec, size_t runtime_nsec);
	/***********/
/******************************/

/*******Constructors*******/
/* Constructs a new Timer object.
 *
 * Parameters:
 * 		runtime_sec: The number of seconds to run.
 * 		runtime_nsec: The number of micro-seconds to run. */
Timer* newTimer(size_t runtime_sec, size_t runtime_nsec);

/* Frees a timer object. */
void freeTimer(Timer* t);
/* Deletes a timer object. */
void delTimer(Timer** t);
/**************************/

#endif
