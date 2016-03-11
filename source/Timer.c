#include "Timer_private.h"

/*******Public Functions*******/
/* Resets the timer and sets the end time of the timer. */
void Timer_begin(Timer* t)
{
	if(!t)return;
	clock_gettime(CLOCK_MONOTONIC, &t->end_time);
	timespec_add(&t->run_time, &t->end_time, &t->end_time);
	t->rang = 0;
}
/* Same as 'Timer_begin()' except it allows you to set the end time
 * to an earlier time. This is useful when you need to ensure the timer
 * rings are caught with more precision as 'Timer_get_time_status()' may take
 * several micro seconds.
 *
 * Parameters:
 * 		t: The object to modify.
 * 		interval: The time interval to subtract from the current time. */
void Timer_begin_before(Timer* t, struct timespec* interval)
{
	if(!t)return;

	Timer_begin(t);
	timespec_subtract(&t->end_time, interval, &t->end_time);
}
/* Checks to see if the timer has rung.  Once a timer has rung, it will
 * stay in the rung state until it is reset by 'Timer_reset()'.
 *
 * Returns:
 * 		>0: Timer has rung.
 * 		 0: Timer has not rung yet.
 * 		<0: alib_error */
int Timer_check(Timer* t)
{
	if(!t)return(ALIB_BAD_ARG);
	if(t->rang)return(t->rang);
	
	int err;
	if((err = Timer_get_time_status(t, NULL, NULL)))
		return(err);
	else
		return(t->rang);
}

	/* Getters */
/* Returns the time that the timer was started.
 *
 * Assumes 't' is not null. */
const struct timespec* Timer_get_end_time(Timer* t){return(&t->end_time);}
/* Returns the real time version of the end time.
 *
 * If an error occurs, function returns a zeroed out timespec. */
struct timespec Timer_get_end_time_real_time(Timer* t)
{
	if(!t)return((struct timespec){0, 0});

	struct timespec mono;
	struct timespec real;

	clock_gettime(CLOCK_MONOTONIC, &mono);
	clock_gettime(CLOCK_REALTIME, &real);

	timespec_subtract(&t->end_time, &mono, &mono);
	timespec_add(&real, &mono, &real);
	return(real);
}
/* Returns the time to wait before ringing the timer.
 *
 * Assumes 't' is not null. */
const struct timespec* Timer_get_run_time(Timer* t){return(&t->run_time);}
/* Returns the last checked rung state of the timer.
 *
 * Assumes 't' is not null. */
char Timer_get_rung_state(Timer* t){return(t->rang);}

/* Returns both the remaining time and the overflow time for the timer.
 * If there is no time remaining, 'rTime' will be set to 0.  If there is
 * time remaining, 'oTime' will be set to 0.
 *
 * Parameters:
 * 		t: The timer to check.
 * 		rTime: (OPTIONAL) The remaining time.
 * 		oTime: (OPTIOINAL) The overflow time.
 *
 * Returns:
 * 		alib_error: If ALIB_UNKNOWN_ERR is returned, then something occurred where
 * 			the calculated time value came out with */
alib_error Timer_get_time_status(Timer* t, struct timespec* rTime,
		struct timespec* oTime)
{
	if(!t)return(ALIB_BAD_ARG);

	struct timespec now;

	if(clock_gettime(CLOCK_MONOTONIC, &now))
		return(ALIB_CHECK_ERRNO);
	timespec_subtract(&t->end_time, &now, &now);
	timespec_fix_values(&now);

	/* Overrun time. */
	if(now.tv_nsec <= 0 && now.tv_sec <= 0)
	{
		t->rang = 1;
		if(rTime)
			memset(rTime, 0, sizeof(struct timespec));

		if(oTime)
		{
			oTime->tv_sec = -now.tv_sec;
			oTime->tv_nsec = -now.tv_nsec;
		}
	}
	/* Time remaining. */
	else if(now.tv_sec >= 0 && now.tv_nsec >= 0)
	{
		if(oTime)
			memset(oTime, 0, sizeof(struct timespec));

		if(rTime)
			*rTime = now;
	}
	/* Error. */
	else
		return(ALIB_UNKNOWN_ERR);

	return(ALIB_OK);
}

	/***********/

	/* Setters */
/* Sets the run time of the timer.
 *
 * Assumes 't' is not null. */
void Timer_set_run_time(Timer* t, size_t runtime_sec, size_t runtime_nsec)
{
	timespec_init(&t->run_time, (long)runtime_sec, (long)runtime_nsec);
}
	/***********/
/******************************/

/*******Constructors*******/
/* Constructs a new Timer object.
 *
 * Parameters:
 * 		runtime_sec: The number of seconds to run.
 * 		runtime_nsec: The number of micro-seconds to run. */
Timer* newTimer(size_t runtime_sec, size_t runtime_nsec)
{
	if(!runtime_sec && !runtime_nsec)return(NULL);

	Timer* t = malloc(sizeof(Timer));
	if(!t)return(NULL);
	
	Timer_set_run_time(t, runtime_sec, runtime_nsec);
	memset(&t->end_time, 0, sizeof(struct timespec));
	t->rang = 1;
	
	return(t);
}

/* Frees a timer object. */
void freeTimer(Timer* t)
{
	if(t)
		free(t);
}
/* Deletes a timer object. */
void delTimer(Timer** t)
{
	if(!t)return;

	freeTimer(*t);
	*t = NULL;
}
/**************************/
