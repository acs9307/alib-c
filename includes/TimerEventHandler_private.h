#ifndef TIMER_EVENT_HANDLER_PRIVATE_IS_DEFINED
#define TIMER_EVENT_HANDLER_PRIVATE_IS_DEFINED

#include "TimerEventHandler.h"
#include "TimerEvent_private.h"

/* Handler for TimerEvents capable of handling multiple timers events at one
 * time.
 *
 * Not suitable for time critical events as it is not reliable to the
 * microsecond, however should be accurate within 100s of micros, at least,
 * depending on the system running the software. */
struct TimerEventHandler
{
	/* Thread members for handling polling of events. */
	pthread_t thread;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	flag_pole fp;

	/* List of events. */
	DList* list;

	/* If true, a new thread will be created then detached for each user callback
	 * made. */
	char threadPerCallback;
};

#endif
