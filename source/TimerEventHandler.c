#include "TimerEventHandler_private.h"

/*******Private Functions*******/
/* Adds an event to the event list.  This is done without locking the mutex
 * as it will be called from within the processing thread.
 *
 * Checks the list from front to back.
 *
 * Parameters:
 * 		handler: The object to modify.
 * 		event: The event to add.  The events timer will be started, if necessary.
 * 		itm: (OPTIONAL) The item that contains 'event'. */
static alib_error add_event(TimerEventHandler* handler, TimerEvent* event, DListItem* itm)
{
	if(!handler || !event)return(ALIB_BAD_ARG);

	TimerEvent* it_event;
	const DListItem* it;

	/* Ensure we have an item to insert. */
	if(!itm)
		itm = newDListItem(event, (alib_free_value)freeTimerEvent, NULL);

	it = DList_get(handler->list, 0);
	if(!it)
	{	/* Nothing exists in the list, just add the event. */
		return(DList_push_back(handler->list, itm));
	}
	it_event = (TimerEvent*)DListItem_get_value(it);

	/* Ensure the timer has been started if the handler is running. */
	if(handler->fp & THREAD_IS_RUNNING)
	{
		if(!*((uint64_t*)Timer_get_end_time(TimerEvent_get_timer(event))))
			Timer_begin(TimerEvent_get_timer(event));

		/* Search for the location to insert into. */
		while(!it_event ||
				(timespec_cmp_fast(Timer_get_end_time(TimerEvent_get_timer(event)),
						Timer_get_end_time(TimerEvent_get_timer(it_event))) > 0))
		{
			it = DListItem_get_next_item(it);
			if(it)
				it_event = DListItem_get_value(it);
			else
				break;
		}
	}
	else
	{
		/* Search for the location to insert into. */
		while(!it_event ||
				(timespec_cmp_fast(Timer_get_run_time(TimerEvent_get_timer(event)),
						Timer_get_run_time(TimerEvent_get_timer(it_event))) > 0))
		{
			it = DListItem_get_next_item(it);
			if(it)
				it_event = DListItem_get_value(it);
			else
				break;
		}
	}

	/* Insert the item. */
	if(it)
	{
		int err = DList_insert_before(handler->list, (DListItem*)it, itm);
		if(err)return(err);
	}
	else
	{
		int err = DList_push_back(handler->list, itm);
		if(err)return(err);
	}

	return(ALIB_OK);
}
/* Removes an event from the handler. */
static void remove_event(TimerEventHandler* handler, TimerEvent* event)
{
	DList_remove_item(handler->list,
	 	DListItem_get_by_value((DListItem*)DList_get(handler->list, 0), event));
	pthread_cond_broadcast(&handler->cond);
}

	/* Callback Functions */
/* Called whenever the timer rings on a child TimerEvent. */
static void timer_rang(TimerEvent* event)
{
	TimerEventHandler* handler = TimerEvent_get_parent(event);

	/* Check to see if we need to make a new thread for the callback. */
	if(handler->threadPerCallback)
	{
		/* The callback takes one argument and returns nothing.  Users
		 * won't be able to get the return value anyhow because we must
		 * detach the thread. */
		pthread_t cbThread;
		if(pthread_create(&cbThread, NULL,
				(pthread_proc)TimerEvent_get_rang_cb(event),
				event) == 0)
		{
			pthread_detach(cbThread);
		}
	}
	/* Call the event on the current thread. */
	else
		TimerEvent_get_rang_cb(event)(event);
}
/* Called whenever a child TimerEvent is preparing to be freed. */
static void event_prep_to_free(TimerEvent* event)
{
	remove_event((TimerEventHandler*)TimerEvent_get_parent(event), event);
}
	/**********************/

	/* Threaded Functions */
/* Loop for checking the timer events.  If an event is fired, then
 * the callback will be called and the event will be placed back into
 * the list where it belongs. */
static void timer_loop(TimerEventHandler* handler)
{
	if(!handler)return;

	DListItem* itm;
	char rang;
	struct timespec eTime;

	flag_raise(&handler->fp, THREAD_IS_RUNNING);
	pthread_mutex_lock(&handler->mutex);
	while(!(handler->fp & THREAD_STOP))
	{
		/* Give the user a chance to add/remove events. */
		pthread_mutex_unlock(&handler->mutex);
		pthread_mutex_lock(&handler->mutex);

		/* Ensure we have events in the list. */
		while(!DList_get_count(handler->list) &&
				!(handler->fp & THREAD_STOP))
			pthread_cond_wait(&handler->cond, &handler->mutex);

		itm = (DListItem*)DList_get(handler->list, 0);
		if(itm)
		{
			eTime = TimerEvent_check(((TimerEvent*)DListItem_get_value(itm)),
					&rang);

			/* Object was deleted, never should have a ring time of 0. */
			if(!*((uint64_t*)&eTime))
				continue;
			else if(rang)
			{
				itm = DList_pull_out(handler->list, DListItem_index(itm));
				if(add_event(handler, (TimerEvent*)DListItem_get_value(itm), itm))
					goto f_return;
			}
			else
			{
				eTime = Timer_get_end_time_real_time(
						TimerEvent_get_timer((TimerEvent*)DListItem_get_value(itm)));
				printf("DEBU1\n");
				pthread_cond_timedwait(&handler->cond, &handler->mutex,
						&eTime);
				printf("DEBUG2\n");
			}
		}
	}
f_return:
	pthread_mutex_unlock(&handler->mutex);
	flag_lower(&handler->fp, THREAD_IS_RUNNING);
}
	/**********************/
/*******************************/

/*******Public Functions*******/
/* Starts the TimerEventHandler on a separate thread. */
alib_error TimerEventHandler_start(TimerEventHandler* handler)
{
	const DListItem* it;

	if(!handler)return(ALIB_BAD_ARG);
	if(handler->fp & THREAD_IS_RUNNING)
		return(ALIB_OK);
	/* Something strange has occurred, the thread
	 * has been created but has not been joined yet.
	 * Stop it and continue. */
	else if(handler->fp & THREAD_CREATED)
		TimerEventHandler_stop(handler);

	/* Restart all the timer events. */
	for(it = DList_get(handler->list, 0); it; it = DListItem_get_next_item(it))
		TimerEvent_begin((TimerEvent*)DListItem_get_value(it));

	/* Start the thread. */
	flag_lower(&handler->fp, THREAD_STOP);
	flag_raise(&handler->fp, THREAD_CREATED);
	if(pthread_create(&handler->thread, NULL, (pthread_proc)timer_loop, handler))
	{
		/* Error occurred while trying to start the thread. */
		flag_lower(&handler->fp, THREAD_CREATED);
		return(ALIB_THREAD_ERR);
	}

	return(ALIB_OK);
}
/* Safely stops the TimerEventHandler.
 *
 * If 'threadPerCallback' is not enabled, then this function
 * may block until all callbacks have returned. */
void TimerEventHandler_stop(TimerEventHandler* handler)
{
	if(!handler)return;

	if(handler->fp & THREAD_CREATED)
	{
		flag_raise(&handler->fp, THREAD_STOP);
		pthread_cond_broadcast(&handler->cond);
		pthread_join(handler->thread, NULL);
		flag_lower(&handler->fp, THREAD_CREATED);
	}
}

/* Adds a TimerEvent to the handler.
 *
 * This may block for an extended period of time if
 * 'threadPerCallback' is not enabled and the currently running callback
 * is taking an extended period of time to return. */
alib_error TimerEventHandler_add_tsafe(TimerEventHandler* handler,
		TimerEvent* event)
{
	if(!handler || !event || TimerEvent_get_parent(event))return(ALIB_BAD_ARG);

	int err = ALIB_OK;

	if(pthread_mutex_lock(&handler->mutex))
		return(ALIB_MUTEX_ERR);

	TimerEvent_set_prep_free_cb(event, event_prep_to_free);
	TimerEvent_set_rang_parent_cb(event, handler, timer_rang);
	err = add_event(handler, event, NULL);
	if(err)goto f_return;

	pthread_cond_broadcast(&handler->cond);

f_return:
	pthread_mutex_unlock(&handler->mutex);
	return(err);
}
/* Removes an event from the handler.
 *
 * This may block for an extended period of time if
 * 'threadPerCallback' is not enabled and the currently running callback
 * is taking an extended period of time to return. */
void TimerEventHandler_remove_tsafe(TimerEventHandler* handler,
		TimerEvent* event)
{
	if(!handler || !event)return;

	if(pthread_mutex_lock(&handler->mutex))
		return;

	/* Remove this from the child. */
	TimerEvent_set_rang_parent_cb(event, NULL, NULL);
	remove_event(handler, event);

	pthread_mutex_unlock(&handler->mutex);
}

	/* Getters */
/* Returns the number of events that the handler is handling. */
size_t TimerEventHandler_get_event_count(TimerEventHandler* handler)
{
	return(DList_get_count(handler->list));
}
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
TimerEventHandler* newTimerEventHandler(char threadPerCallback)
{
	TimerEventHandler* handler =
			(TimerEventHandler*)malloc(sizeof(TimerEventHandler));
	if(!handler)return(NULL);

	/* Init non-dynamic members. */
	handler->threadPerCallback = threadPerCallback;
	handler->mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
	handler->cond = (pthread_cond_t)PTHREAD_COND_INITIALIZER;
	handler->fp = FLAG_INIT;

	/* Init dynamic members. */
	handler->list = newDList();

	/* Check initialization. */
	if(!handler->list)
		delTimerEventHandler(&handler);

	return(handler);
}
/**************************/

/*******Destructors*******/
/* Frees the handler. */
void freeTimerEventHandler(TimerEventHandler* handler)
{
	if(!handler)return;

	flag_raise(&handler->fp, OBJECT_DELETE_STATE);
	TimerEventHandler_stop(handler);

	delDList(&handler->list);
	pthread_cond_destroy(&handler->cond);
	pthread_mutex_destroy(&handler->mutex);
	free(handler);
}
/* Frees the handler and sets the pointer to NULL. */
void delTimerEventHandler(TimerEventHandler** handler)
{
	if(!handler)return;

	freeTimerEventHandler(*handler);
	*handler = NULL;
}
/*************************/
