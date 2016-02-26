#ifndef THREADED_TIMER_EVENT_IS_DEFINED
#define THREADED_TIMER_EVENT_IS_DEFINED

#include <pthread.h>
#include <stdlib.h>

#include "TimerEvent.h"

typedef struct ThreadedTimerEvent ThreadedTimerEvent;

/*******Public Functions*******/
/* Starts running the thread for the ThreadedTimerEvent.
 *
 * May block if a previous call to 'ThreadedTimerEvent_stop_async()'
 * was made. */
alib_error ThreadedTimerEvent_start(ThreadedTimerEvent* event);
/* Stops the ThreadedTimerEvent.
 *
 * If the object is currently in a callback state, then
 * 'ThreadedTimerEvent_stop_async()' will be called instead.
 *
 * This may block until the event callback returns. */
void ThreadedTimerEvent_stop(ThreadedTimerEvent* event);
/* Calls for the ThreadedTimerEvent to stop.
 *
 * Unlike 'ThreadedTimerEvent_stop()' this will not block. */
void ThreadedTimerEvent_stop_async(ThreadedTimerEvent* event);

/* Blocks until an event has been raised before returning. */
void ThreadedTimerEvent_wait(ThreadedTimerEvent* event);

	/* Getters */
/* Returns whether or not the event timer is running. */
char ThreadedTimerEvent_is_running(ThreadedTimerEvent* event);
	/***********/
/******************************/

/*******Constructors*******/
/* Initializes a ThreadedTimerEvent. */
void initThreadedTimerEvent(ThreadedTimerEvent* event, size_t sec, size_t nsec, TimerEvent_rang_cb rang_cb,
		void* exData, alib_free_value freeExData);
/* Creates a new ThreadedTimerEvent. */
ThreadedTimerEvent* newThreadedTimerEvent(size_t sec, size_t nsec, TimerEvent_rang_cb rang_cb,
		void* exData, alib_free_value freeExData);
/**************************/

/*******Destructors*******/
/* Frees a ThreadedTimerEvent. */
void freeThreadedTimerEvent(ThreadedTimerEvent* event);
/* Deletes a ThreadedTimerEvent. */
void delThreadedTimerEvent(ThreadedTimerEvent** event);
/*************************/

#endif
