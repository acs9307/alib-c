#ifndef TIMER_LIGHT_IS_DEFINED
#define TIMER_LIGHT_IS_DEFINED

#include "alib_error.h"
#include "time.h"
#include "TimerBase.h"

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
typedef struct TimerLight TimerLight;

/*******Public Functions*******/
/* Resets the timer and sets the start time of the timer. */
void TimerLight_begin(TimerLight* t);
/* Checks to see if the timer has rung.  Once a timer has rung, it will
 * stay in the rung state until it is reset by 'Timer_reset()'.
 *
 * Returns:
 * 		!0: Timer has rung.
 * 		 0: Timer has not rung yet. */
char TimerLight_check(TimerLight* t);

	/* Getters */
/* Returns the time that the timer was started.
 *
 * Assumes 't' is not null. */
size_t TimerLight_get_start_time(TimerLight* t);
/* Returns the time to wait before ringing the timer.
 *
 * Assumes 't' is not null. */
size_t TimerLight_get_run_time(TimerLight* t);
/* Returns what magnitude the timer stores time in.
 *
 * Assumes 't' is not null. */
MagnitudeTime TimerLight_get_magnitude(TimerLight* t);
	/***********/

	/* Setters */
/* Sets the run time of the timer.
 *
 * Assumes 't' is not null. */
void TimerLight_set_run_time(TimerLight* t, size_t run_time);
/* Sets the magnitude of the timer.
 *
 * The timer's start and run time will be updated to the new magnitude.
 * WARNING: Overflow may occur, depending on the set internal values. */
void TimerLight_set_magnitude(TimerLight* t, MagnitudeTime mag);
	/***********/
/******************************/

/*******Constructors*******/
/* Constructs a new Timer object. */
TimerLight* newTimerLight(uint64_t run_time, MagnitudeTime mag);
/* Deletes a timer object. */
void delTimerLight(TimerLight** t);
/**************************/

#endif
