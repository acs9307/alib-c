#ifndef TIMER_EVENT_IS_DEFINED
#define TIMER_EVENT_IS_DEFINED

#include <limits.h>

#include "alib_types.h"
#include "flags.h"
#include "Timer.h"

/* An object that will call a callback whenever a timer has rung. */
typedef struct TimerEvent TimerEvent;

/* Called whenever the timer has rung. */
typedef void(*TimerEvent_rang_cb)(TimerEvent* event);
/* Called whenever the object is preparing to be deleted so that
 * a parent may be notified. */
typedef void(*TimerEvent_prep_free_cb)(TimerEvent* event);
/* ???Called whenever a value has been changed within the object. */
//typedef void(*TimerEvent_value_changed_cb)(TimerEvent* event, void* value);

/*******Public Functions*******/
/* Checks to see if the timer has rung.  If so, will call the
 * event callback.
 *
 * Returns a timespec struct of the next ring time. */
struct timespec TimerEvent_check(TimerEvent* event, char* rang);

/* Begins running a TimerEvent. */
void TimerEvent_begin(TimerEvent* event);

/* Compares two TimerEvent objects.
 *
 * Returns:
 * 		<0: te1's end time is before te2's end time.
 * 		 0: te1's end time is the same as te2's end time.
 * 		>0: te1's end time is after te2's end time. */
int TimerEvent_compare_least_remaining_time(const TimerEvent* te1, const TimerEvent* te2);

	/* Getters */
/* Returns the Timer object of the TimerEvent.
 *
 * Assumes 'event' is not null. */
Timer* TimerEvent_get_timer(TimerEvent* event);
/* Returns the extended data for the TimerEvent.
 *
 * Assumes 'event' is not null. */
void* TimerEvent_get_ex_data(TimerEvent* event);
/* Returns the rang callback for the TimerEvent.
 *
 * Assumes 'event' is not null. */
TimerEvent_rang_cb TimerEvent_get_rang_cb(TimerEvent* event);
/* Returns the flag pole for the TimerEvent.
 *
 * Assumes 'event' is not null. */
flag_pole TimerEvent_get_flags(TimerEvent* event);
	/***********/

	/* Setters */
/* Sets the extended data for the TimerEvent.
 *
 * Parameters:
 * 		event: The TimerEvent to modify.
 * 		exData: The data to set for the extended data.
 * 		freeExData: The function pointer for freeing 'exData' when the object
 * 			is destroyed.
 * 		freeOldData: If !0, the object will attempt to free the old extended data. */
void TimerEvent_set_ex_data(TimerEvent* event, void* exData,
		alib_free_value freeExData, char freeOldData);
/* Sets the rang callback for the TimerEvent.
 *
 * Assumes 'event' is not null.  */
void TimerEvent_set_rang_cb(TimerEvent* event, TimerEvent_rang_cb rang_cb);
/* Sets the 'prep_free_cb' callback for the TimerEvent.  This will be called
 * whenever the object is preparing to be freed.  Should only be used by parents
 * that must do some cleanup with the object before it is freed.
 *
 * Assumes 'event' is not null. */
void TimerEvent_set_prep_free_cb(TimerEvent* event,
		TimerEvent_prep_free_cb prep_free_cb);
	/***********/
/******************************/

/*******Constructors*******/
/* Initializes a TimerEvent. */
/* Creates a new TimerEvent object.
 * If creation fails, 'exData' will not be freed, even if
 * 'freeExData' is provided.
 *
 * Returns:
 * 		TimerEvent*: Success
 * 		NULL: Failure */
TimerEvent* newTimerEvent(size_t sec, size_t nsec, TimerEvent_rang_cb rang_cb,
		void* exData, alib_free_value freeExData);
/* Creates a new TimerEvnet object.
 * 'timer' is created externally and 'refTimer' should define whether or not the
 * memory of 'timer' is being handled externally or internally of the TimerEvent object.
 *
 * If 'refTimer' is false, 'timer' will be freed upon object deletion.  If it is true,
 * 'timer' will not be freed internally and must be freed by an external object.  If
 * 'timer' is referenced from an external source, 'timer's lifetime must outlast that
 * of the new TimerEvent object or behavior is undefined.
 *
 * Returns:
 * 		TimerEvent*: Success
 * 		NULL: Failure */
TimerEvent* newTimerEvent_ex(Timer* timer, char refTimer, TimerEvent_rang_cb rang_cb,
		void* exData, alib_free_value freeExData);
/**************************/

/*******Destructors*******/
/* Frees the object. */
void freeTimerEvent(TimerEvent* event);
/* Deletes the object. */
void delTimerEvent(TimerEvent** event);
/*************************/

#endif
