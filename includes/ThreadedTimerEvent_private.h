#ifndef THREADED_TIMER_EVENT_PRIVATE_IS_DEFINED
#define THREADED_TIMER_EVENT_PRIVATE_IS_DEFINED

#include "ThreadedTimerEvent.h"
#include "TimerEvent_private.h"

#define THREADED_TIMER_EVENT_MEMBERS 											\
	TIMER_EVENT_MEMBERS;														\
																				\
	pthread_t thread;															\
	pthread_mutex_t mutex;														\
	pthread_cond_t cond;

/* A TimerEvent that runs on an independent thread.  When started, this will wait for the specified time
 * and then calls the event callbacks.  This is a better alternative to 'TimerEventHandler' if only a single
 * timer is required, or a more precise timer is required.
 *
 * The thread will block until the timer event function returns.  This means that it is possible that if
 * the timer is set to go off on too short an interval, overrun may be caused and your function will be
 * called immediately after it returns.
 *
 * Inherits from TimerEvent. */
struct ThreadedTimerEvent
{
	THREADED_TIMER_EVENT_MEMBERS;
};



#endif
