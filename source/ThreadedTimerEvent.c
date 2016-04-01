#include "ThreadedTimerEvent_private.h"

/*******Private Functions*******/
/* Main loop for the thread. */
static void timer_loop(ThreadedTimerEvent* event)
{
	if(!event)return;

	char rang;
	struct timespec end_time;

	flag_raise(&event->fp, THREAD_IS_RUNNING);
	while(!(event->fp & THREAD_STOP))
	{
		TimerEvent_check((TimerEvent*)event, &rang);

		/* Let anyone waiting know that we have rung. */
		if(rang)
			pthread_cond_broadcast(&event->cond);

		if(pthread_mutex_lock(&event->mutex))
				flag_raise(&event->fp, THREAD_STOP);
		end_time = Timer_get_end_time_real_time(event->timer);
		pthread_cond_timedwait(&event->cond, &event->mutex,
				&end_time);

		pthread_mutex_unlock(&event->mutex);
	}
	flag_lower(&event->fp, THREAD_IS_RUNNING);

	pthread_cond_broadcast(&event->cond);
	pthread_cond_broadcast(&event->cond);
}
/*******************************/

/*******Public Functions*******/
/* Starts running the thread for the ThreadedTimerEvent.
 *
 * May block if a previous call to 'ThreadedTimerEvent_stop_async()'
 * was made. */
alib_error ThreadedTimerEvent_start(ThreadedTimerEvent* event)
{
	if(!event)return(ALIB_BAD_ARG);
	if((event->fp & THREAD_IS_RUNNING) && !(event->fp & THREAD_STOP))
		return(ALIB_OK);
	else if(event->fp & THREAD_CREATED)
		ThreadedTimerEvent_stop(event);
	/* Thread has been detached, but has not returned yet. */
	else
	{
		if(pthread_mutex_lock(&event->mutex))
			return(ALIB_MUTEX_ERR);
		while((event->fp & THREAD_IS_RUNNING) && (event->fp & THREAD_STOP))
			pthread_cond_wait(&event->cond, &event->mutex);
		pthread_mutex_unlock(&event->mutex);
	}

	Timer_begin(event->timer);
	/* Start the thread. */
	flag_lower(&event->fp, THREAD_STOP);
	flag_raise(&event->fp, THREAD_CREATED);
	if(pthread_create(&event->thread, NULL, (pthread_proc)timer_loop, event))
	{
		/* Error occurred while trying to start the thread. */
		flag_lower(&event->fp, THREAD_CREATED);
		return(ALIB_THREAD_ERR);
	}

	return(ALIB_OK);
}
/* Stops the ThreadedTimerEvent.
 *
 * If the object is currently in a callback state, then
 * 'ThreadedTimerEvent_stop_async()' will be called instead.
 *
 * This may block until the event callback returns. */
void ThreadedTimerEvent_stop(ThreadedTimerEvent* event)
{
	if(!event)return;

	if(event->fp & OBJECT_CALLBACK_STATE)
		ThreadedTimerEvent_stop_async(event);
	else if(event->fp & THREAD_CREATED)
	{
		flag_raise(&event->fp, THREAD_STOP);
		pthread_cond_broadcast(&event->cond);
		pthread_join(event->thread, NULL);
		flag_lower(&event->fp, THREAD_CREATED);
	}
}
/* Calls for the ThreadedTimerEvent to stop.
 *
 * Unlike 'ThreadedTimerEvent_stop()' this will not block. */
void ThreadedTimerEvent_stop_async(ThreadedTimerEvent* event)
{
	if(!event)return;

	if(event->fp & THREAD_CREATED)
	{
		flag_raise(&event->fp, THREAD_STOP);
		pthread_cond_broadcast(&event->cond);
		pthread_detach(event->thread);
		flag_lower(&event->fp, THREAD_CREATED);
	}
}

/* Blocks until an event has been raised. Returns immediately
 * if the event is not running. */
void ThreadedTimerEvent_wait(ThreadedTimerEvent* event)
{
	if(event->fp & THREAD_IS_RUNNING)
	{
		if(pthread_mutex_lock(&event->mutex))
			return;
		pthread_cond_wait(&event->cond, &event->mutex);
		pthread_mutex_unlock(&event->mutex);
	}
}

	/* Getters */
/* Returns whether or not the event timer is running. */
char ThreadedTimerEvent_is_running(ThreadedTimerEvent* event){return(event->fp & THREAD_IS_RUNNING);}
	/***********/
/******************************/

/*******Constructors*******/
/* Initializes a ThreadedTimerEvent. */
void initThreadedTimerEvent(ThreadedTimerEvent** event, Timer* timer, char refTimer, TimerEvent_rang_cb rang_cb,
		void* exData, alib_free_value freeExData)
{
	if(!event)return;

	initTimerEvent((TimerEvent**)event, timer, refTimer, rang_cb, exData, freeExData);

	if(event)
	{
		(*event)->mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
		(*event)->cond = (pthread_cond_t)PTHREAD_COND_INITIALIZER;
		(*event)->freeInheritor = (TimerEvent_prep_free_cb)freeThreadedTimerEvent;
	}
}

/* Creates a new ThreadedTimerEvent.
 *
 * If 'timer' is referenced (its memory is handled external to this object), then 'refTimer' must be true.
 * If 'refTimer' is not true, then 'timer' will be freed when the returned object is freed.
 * If 'refTimer' is false, then 'timer' must outlive the ThreadedTimerEvent or behavior is undefined.
 *
 * If NULL is returned, 'timer' will be freed if 'refTimer' is false. */
ThreadedTimerEvent* newThreadedTimerEvent_ex(Timer* timer, char refTimer, TimerEvent_rang_cb rang_cb,
		void* exData, alib_free_value freeExData)
{
	ThreadedTimerEvent* event = (ThreadedTimerEvent*)malloc(sizeof(ThreadedTimerEvent));
	initThreadedTimerEvent(&event, timer, refTimer, rang_cb, exData, freeExData);
	return(event);
}
/* Creates a new ThreadedTimerEvent. */
ThreadedTimerEvent* newThreadedTimerEvent(size_t sec, size_t nsec, TimerEvent_rang_cb rang_cb,
		void* exData, alib_free_value freeExData)
{
	return(newThreadedTimerEvent_ex(newTimer(sec, nsec), 0, rang_cb, exData, freeExData));
}
/**************************/

/*******Destructors*******/
/* Frees a ThreadedTimerEvent. */
void freeThreadedTimerEvent(ThreadedTimerEvent* event)
{
	if(!event)return;

	flag_raise(&event->fp, OBJECT_DELETE_STATE);
	ThreadedTimerEvent_stop(event);
	pthread_mutex_destroy(&event->mutex);
	pthread_cond_destroy(&event->cond);

	event->freeInheritor = NULL;
	freeTimerEvent((TimerEvent*)event);
}
/* Deletes a ThreadedTimerEvent. */
void delThreadedTimerEvent(ThreadedTimerEvent** event)
{
	if(!event)return;

	freeThreadedTimerEvent(*event);
	*event = NULL;
}
/*************************/
