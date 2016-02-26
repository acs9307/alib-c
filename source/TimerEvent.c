#include "TimerEvent_private.h"

/*******Protected Functions*******/
	/* Getters */
/* Returns a pointer to the parent of the TimerEvent.
 *
 * Assumes 'event' is not null. */
void* TimerEvent_get_parent(TimerEvent* event){return(event->parent);}
	/***********/

	/* Setters */
/* Sets the 'rang_parent_cb' for the TimerEvent.  This should be used by parent
 * objects, such as event handlers, that need control over the behavior of the
 * object before and after calling the user's callback.
 *
 * Assumes 'event' is not null. */
void TimerEvent_set_rang_parent_cb(TimerEvent* event,
		void* parent, TimerEvent_rang_cb rang_parent_cb)
{
	event->parent = parent;
	event->rang_parent_cb = rang_parent_cb;
}
	/***********/
/*********************************/

/*******Public Functions*******/
/* Checks to see if the timer has rung.  If so, will call the
 * event callback.
 *
 * In the event that the TimerEvent has free called on it while in the
 * callback, the returned value will be {0, 0}.
 *
 * Returns a timespec struct of the next ring time. */
struct timespec TimerEvent_check(TimerEvent* event, char* rang)
{
	struct timespec rTime, oTime;
	if(Timer_get_time_status(event->timer, &rTime, &oTime))
	{	/* Error occurred, try restarting the timer. */
		rTime = *Timer_get_run_time(event->timer);
		Timer_begin(event->timer);

		if(rang)
			*rang = 0;
		goto f_return;
	}
	/* If there is remaining time, then just return the remaining time. */
	else if(rTime.tv_nsec || rTime.tv_sec)
	{
		if(rang)
			*rang = 0;
		goto f_return;
	}
	else
		Timer_begin_before(event->timer, &oTime);

	/* Ensure we have a callback for when the event rings. */
	if(event->rang_parent_cb || event->rang_cb)
	{
		if(rang)*rang = 1;

		flag_raise(&event->fp, OBJECT_CALLBACK_STATE);
		if(event->rang_parent_cb)
			event->rang_parent_cb(event);
		else
			event->rang_cb(event);
		flag_lower(&event->fp, OBJECT_CALLBACK_STATE);

		if(event->fp & OBJECT_DELETE_STATE)
		{
			freeTimerEvent(event);
			return((struct timespec){0, 0});
		}
	}

f_return:
	return(*Timer_get_end_time(event->timer));
}

/* Begins running a TimerEvent. */
void TimerEvent_begin(TimerEvent* event)
{
	if(event)
		Timer_begin(event->timer);
}

	/* Getters */
/* Returns the Timer object of the TimerEvent.
 *
 * Assumes 'event' is not null. */
Timer* TimerEvent_get_timer(TimerEvent* event){return(event->timer);}
/* Returns the extended data for the TimerEvent.
 *
 * Assumes 'event' is not null. */
void* TimerEvent_get_ex_data(TimerEvent* event){return(event->exData);}
/* Returns the rang callback for the TimerEvent.
 *
 * Assumes 'event' is not null. */
TimerEvent_rang_cb TimerEvent_get_rang_cb(TimerEvent* event){return(event->rang_cb);}
/* Returns the flag pole for the TimerEvent.
 *
 * Assumes 'event' is not null. */
flag_pole TimerEvent_get_flags(TimerEvent* event){return(event->fp);}
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
		alib_free_value freeExData, char freeOldData)
{
	if(!event)return;

	/* Free the old pointed to value. */
	if(freeOldData && event->exData && event->freeExData)
		event->freeExData(event->exData);

	event->exData = exData;
	event->freeExData = freeExData;
}
/* Sets the rang callback for the TimerEvent.
 *
 * Assumes 'event' is not null.  */
void TimerEvent_set_rang_cb(TimerEvent* event, TimerEvent_rang_cb rang_cb)
{
	event->rang_cb = rang_cb;
}
/* Sets the 'prep_free_cb' callback for the TimerEvent.  This will be called
 * whenever the object is preparing to be freed.  Should only be used by parents
 * that must do some cleanup with the object before it is freed.
 *
 * Assumes 'event' is not null. */
void TimerEvent_set_prep_free_cb(TimerEvent* event,
		TimerEvent_prep_free_cb prep_free_cb)
{
	event->prep_free_cb = prep_free_cb;
}
	/***********/
/******************************/

/*******Constructors*******/
/* Initializes a TimerEvent. */
void initTimerEvent(TimerEvent* event, size_t sec, size_t nsec, TimerEvent_rang_cb rang_cb,
		void* exData, alib_free_value freeExData)
{
	if(!event)return;

	/* Initialize non-dyanmic members. */
	event->exData = exData;
	event->freeExData = freeExData;
	event->rang_cb = rang_cb;
	event->prep_free_cb = NULL;
	event->fp = FLAG_INIT;
	event->parent = NULL;
	event->freeInheritor = NULL;

	/* Initialize dynamic members. */
	event->timer = newTimer(sec, nsec);
	if(!event->timer)
		delTimerEvent(&event);

}
/* Creates a new TimerEvent object.
 * If creation fails, 'exData' will not be freed, even if
 * 'freeExData' is provided.
 *
 * Returns:
 * 		TimerEvent*: Success
 * 		NULL: Failure */
TimerEvent* newTimerEvent(size_t sec, size_t nsec, TimerEvent_rang_cb rang_cb,
		void* exData, alib_free_value freeExData)
{
	TimerEvent* event = (TimerEvent*)malloc(sizeof(TimerEvent));
	initTimerEvent(event, sec, nsec, rang_cb, exData, freeExData);
	return(event);
}
/**************************/

/*******Destructors*******/
/* Frees the object. */
void freeTimerEvent(TimerEvent* event)
{
	if(!event)return;

	flag_raise(&event->fp, OBJECT_DELETE_STATE);

	/* Check to see if it is safe to free the object. */
	if(!(event->fp & OBJECT_CALLBACK_STATE))
	{
		flag_raise(&event->fp, OBJECT_CALLBACK_STATE);
		if(event->prep_free_cb)
			event->prep_free_cb(event);

		if(event->freeInheritor)
			event->freeInheritor(event);
		flag_lower(&event->fp, OBJECT_CALLBACK_STATE);

		freeTimer(event->timer);
		if(event->exData && event->freeExData)
			event->freeExData(event->exData);
		free(event);
	}
}
/* Deletes the object. */
void delTimerEvent(TimerEvent** event)
{
	if(!event)return;

	freeTimerEvent(*event);
	*event = NULL;
}
/*************************/
