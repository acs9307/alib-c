#ifndef TIMER_EVENT_PRIVATE_IS_DEFINED
#define TIMER_EVENT_PRIVATE_IS_DEFINED

#include "TimerEvent.h"

#define TIMER_EVENT_MEMBERS 													\
	/* Timer for the event. */ 													\
	Timer* timer; 																\
	/* 0 if 'timer' was created internally, !0 if it was created externally. */	\
	char refTimer;																\
																				\
	/* Extended user data. */ 													\
	void* exData; 																\
	alib_free_value freeExData; 												\
																				\
	/* User callback for when the timer rings. */ 								\
	TimerEvent_rang_cb rang_cb; 												\
																				\
	/* Parent callback for whenever the TimerEvent is preparing to be 			\
	 * freed. */ 																\
	TimerEvent_prep_free_cb prep_free_cb; 										\
	/* Should only be set by parents of the TimerEvent, specifically 			\
	 * handlers. If this is set, then this will be called by the object 		\
	 * instead of 'rang_cb'. It is the responsibility of the parent to			\
	 * call 'rang_cb'. */														\
	TimerEvent_rang_cb rang_parent_cb;											\
	void* parent;																\
																				\
	/* Callback for freeing any objects that inherit from this object. */		\
	TimerEvent_prep_free_cb freeInheritor;										\
	/* Status flagpole. */														\
	flag_pole fp

/* An object that will call a callback whenever a timer has rung. */
struct TimerEvent
{
	TIMER_EVENT_MEMBERS;
};

/*******Protected Functions*******/
	/* Getters */
/* Returns a pointer to the parent of the TimerEvent.
 *
 * Assumes 'event' is not null. */
void* TimerEvent_get_parent(TimerEvent* event);
	/***********/

	/* Setters */
/* Sets the 'rang_parent_cb' for the TimerEvent.  This should be used by parent
 * objects, such as event handlers, that need control over the behavior of the
 * object before and after calling the user's callback.
 *
 * Assumes 'event' is not null. */
void TimerEvent_set_rang_parent_cb(TimerEvent* event,
		void* parent, TimerEvent_rang_cb rang_parent_cb);
	/***********/

	/* Constructors */
void initTimerEvent(TimerEvent** event, Timer* timer, char refTimer, TimerEvent_rang_cb rang_cb,
		void* exData, alib_free_value freeExData);
	/****************/
/*********************************/

#endif
