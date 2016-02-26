#ifndef TIMER_EVENT_HANDLER_IS_DEFINED
#define TIMER_EVENT_HANDLER_IS_DEFINED

#include <pthread.h>
#include <errno.h>

#include "DList.h"
#include "flags.h"
#include "alib_types.h"
#include "TimerEvent.h"

/* Handler for TimerEvents capable of handling multiple timers events at one
 * time.
 *
 * Not suitable for time critical events as it is not reliable to the
 * microsecond, however should be accurate within 100s of micros, at least,
 * depending on the system running the software. */
typedef struct TimerEventHandler TimerEventHandler;

/*******Public Functions*******/
/* Starts the TimerEventHandler on a separate thread. */
alib_error TimerEventHandler_start(TimerEventHandler* handler);
/* Safely stops the TimerEventHandler.
 *
 * If 'threadPerCallback' is not enabled, then this function
 * may block until all callbacks have returned. */
void TimerEventHandler_stop(TimerEventHandler* handler);

/* Adds a TimerEvent to the handler.
 *
 * This may block for an extended period of time if
 * 'threadPerCallback' is not enabled and the currently running callback
 * is taking an extended period of time to return. */
alib_error TimerEventHandler_add_tsafe(TimerEventHandler* handler,
		TimerEvent* event);
/* Removes an event from the handler.
 *
 * This may block for an extended period of time if
 * 'threadPerCallback' is not enabled and the currently running callback
 * is taking an extended period of time to return. */
void TimerEventHandler_remove_tsafe(TimerEventHandler* handler,
		TimerEvent* event);

	/* Getters */
/* Returns the number of events that the handler is handling. */
size_t TimerEventHandler_get_event_count(TimerEventHandler* handler);
	/***********/
/******************************/

/*******Constructors*******/
/* Creates a new TimerEventHandler object.
 *
 * Parameters:
 * 		threadPerCallback: If true, the handler will create a new thread
 * 			each time an event's callback is called. This is good if
 * 			a callback takes a long time to return as it will not block
 * 			any other timer events from being raised. Created threads
 * 			are detached and do not need to be joined. */
TimerEventHandler* newTimerEventHandler(char threadPerCallback);
/**************************/

/*******Destructors*******/
/* Frees the handler. */
void freeTimerEventHandler(TimerEventHandler* handler);
/* Frees the handler and sets the pointer to NULL. */
void delTimerEventHandler(TimerEventHandler** handler);
/*************************/

#endif
