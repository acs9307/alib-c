#include "proc_waiter.h"

#include "ArrayList_protected.h"

/*******Private Globals*******/
/* Thread should always be detached and should
 * never be joined. */
pthread_t* PROC_WAITER_THREAD = NULL;
pthread_cond_t* PROC_WAITER_T_COND = NULL;
pthread_mutex_t* PROC_WAITER_MUTEX = NULL;
flag_pole PROC_WAITER_FLAG_POLE = 0;

ArrayList* PROC_WAITER_CB_LIST = NULL;
size_t PROC_WAITER_SLEEP_TIME = 1000 * 1000;  //In microseconds
/*****************************/

/*******Private Structs*******/
/* Used to store data on the PID to watch. */
typedef struct proc_waiter
{
	int pid;
	proc_exited_cb cb;
	void* user_data;
}proc_waiter;
/*****************************/

/*******Private Functions*******/
static void* thread_proc(void* unused)
{
	int status;
	int pid;
	proc_waiter** pw_it;
	size_t pw_it_count;

	flag_raise(&PROC_WAITER_FLAG_POLE, THREAD_IS_RUNNING);
	while(!(PROC_WAITER_FLAG_POLE & THREAD_STOP))
	{
		pid = wait(&status);

		/* There are no child processes connected to the current process. */
		if(pid < 0)
		{
			if(!ArrayList_get_count(PROC_WAITER_CB_LIST))
			{
				pthread_mutex_lock(PROC_WAITER_MUTEX);
				while(pid < 0 && !(PROC_WAITER_FLAG_POLE & THREAD_STOP))
					pthread_cond_wait(PROC_WAITER_T_COND, PROC_WAITER_MUTEX);
				pthread_mutex_unlock(PROC_WAITER_MUTEX);
			}
			else
				usleep(PROC_WAITER_SLEEP_TIME);
			continue;
		}

		/* Iterate through the list and call the registered callbacks. */
		for(pw_it = (proc_waiter**)ArrayList_get_array_ptr(PROC_WAITER_CB_LIST), pw_it_count = 0;
				pw_it_count < ArrayList_get_count(PROC_WAITER_CB_LIST);++pw_it)
		{
			if(*pw_it)
			{
				++pw_it_count;

				/* Call the registered callback. */
				if(((*pw_it)->pid < 0 || (*pw_it)->pid == pid))
				{
					if((*pw_it)->cb)
						(*pw_it)->cb(pid, status, (*pw_it)->user_data);

					/* Deregister an event callback if the pid is not a wildcard
					 * number. */
					if((*pw_it)->pid > -1)
						ArrayList_remove_by_ptr(PROC_WAITER_CB_LIST, (void**)pw_it);
				}
			}
		}
	}

	flag_lower(&PROC_WAITER_FLAG_POLE, THREAD_IS_RUNNING);
	pthread_cond_broadcast(PROC_WAITER_T_COND);
	return(NULL);
}

static alib_error allocate_globals()
{
	/* Init the list. */
	if(!PROC_WAITER_CB_LIST)
	{
		PROC_WAITER_CB_LIST = newArrayList(free);
		if(!PROC_WAITER_CB_LIST)
			return(ALIB_MEM_ERR);
	}

	/* Init the thread. */
	if(!PROC_WAITER_THREAD)
	{
		PROC_WAITER_THREAD = malloc(sizeof(pthread_t));
		if(!PROC_WAITER_THREAD)
			return(ALIB_MEM_ERR);
	}

	/* Init the mutex. */
	if(!PROC_WAITER_MUTEX)
	{
		PROC_WAITER_MUTEX = malloc(sizeof(pthread_mutex_t));
		if(!PROC_WAITER_MUTEX)
			return(ALIB_MEM_ERR);
		else
			pthread_mutex_init(PROC_WAITER_MUTEX, NULL);
	}

	/* Init the thread condition. */
	if(!PROC_WAITER_T_COND)
	{
		PROC_WAITER_T_COND = malloc(sizeof(pthread_cond_t));
		if(!PROC_WAITER_T_COND)
			return(ALIB_MEM_ERR);
		else
			pthread_cond_init(PROC_WAITER_T_COND, NULL);
	}

	return(ALIB_OK);
}
/*******************************/

/* Calls for the process waiter to stop, but it will
 * continue to run until at least one process exits. */
void proc_waiter_stop()
{
	if(!PROC_WAITER_THREAD)return;

	/* Raise the flag to stop the thread, this should stop
	 * the thread whenever a child process closes. */
	flag_raise(&PROC_WAITER_FLAG_POLE, THREAD_STOP);

	/* Wake up the thread if it is waiting for a condition change. */
	if(PROC_WAITER_FLAG_POLE & THREAD_IS_RUNNING)
		pthread_cond_broadcast(PROC_WAITER_T_COND);
}
/* Calls for the waiting thread to stop and then waits
 * for it to return.
 *
 * WILL BLOCK.*/
void proc_waiter_stop_wait()
{
	proc_waiter_stop();

	pthread_mutex_lock(PROC_WAITER_MUTEX);
	while(PROC_WAITER_FLAG_POLE & THREAD_IS_RUNNING)
		pthread_cond_wait(PROC_WAITER_T_COND, PROC_WAITER_MUTEX);
	pthread_mutex_unlock(PROC_WAITER_MUTEX);
}
/* Forces the process waiter to stop immediately.
 * It is suggested to use 'proc_waiter_stop()' or
 * 'proc_waiter_stop_wait()' instead as this will create
 * a new process that immediately exits so that the call to
 * 'wait()' will no longer block. */
void proc_waiter_stop_now()
{
	proc_waiter_stop();

	/* If the thread is running, then we need to fork
	 * a new process then close it immediately. */
	if(PROC_WAITER_FLAG_POLE & THREAD_IS_RUNNING)
	{
		if(!fork())
			exit(0);
		else
			proc_waiter_stop_wait();
	}
}

/* Starts the thread.  If the thread is already running
 * 'ALIB_OK' will be returned.
 *
 * Waiting for child processes to close is handled on a separate thread. */
alib_error proc_waiter_start()
{
	int err = allocate_globals();
	if(err)return(err);

	/* Setup the flags as needed. */
	flag_lower(&PROC_WAITER_FLAG_POLE, THREAD_STOP);

	/* Create the thread. */
	pthread_mutex_lock(PROC_WAITER_MUTEX);

	/* Check to see if we are still running. */
	if(PROC_WAITER_FLAG_POLE & THREAD_IS_RUNNING)
	{
		err = ALIB_OK;
		goto f_unlock;
	}

	/* Raise the THREAD_IS_RUNNING flag before creating the thread so that the flag
	 * will never misrepresent the state of the thread. */
	flag_raise(&PROC_WAITER_FLAG_POLE, THREAD_IS_RUNNING);
	if(pthread_create(PROC_WAITER_THREAD, NULL, thread_proc, NULL))
	{
		flag_lower(&PROC_WAITER_FLAG_POLE, THREAD_IS_RUNNING);
		err = ALIB_THREAD_ERR;
		goto f_unlock;
	}

	/* Detach the thread. */
	if(pthread_detach(*PROC_WAITER_THREAD))
	{
		proc_waiter_stop_now();
		pthread_join(*PROC_WAITER_THREAD, NULL);
		err = ALIB_THREAD_ERR;
		goto f_unlock;
	}
f_unlock:
	pthread_mutex_unlock(PROC_WAITER_MUTEX);

	return(err);
}

/* Registers a callback with the process waiter, but will not start the process waiter
 * if it is not running.  Note that this will not stop the process waiter if it is already
 * running.
 *
 * Callbacks will be removed if their watched pid is greater than -1 and the received
 * pid from 'wait()' equals the watched pid.  If the watched pid is less than 0, then
 * the callback will never be removed from the event list unless manually removed by
 * calling 'proc_waiter_deregister()'.
 *
 * Parameters:
 * 		pid: The process ID returned by 'wait()' that should call 'proc_exited' callback.
 * 			If <0, the callback will be called for any PID that is returned by 'wait()'.
 * 		proc_exited: The callback called when 'wait()' returns a PID that matches 'pid'.
 * 		user_data: Data to be passed to the callback when called. */
alib_error proc_waiter_register_no_start(int pid, proc_exited_cb proc_exited, void* user_data)
{
	int err;
	proc_waiter* pw = malloc(sizeof(proc_waiter));
	if(!pw)return(ALIB_MEM_ERR);

	pw->pid = pid;
	pw->cb = proc_exited;
	pw->user_data = user_data;

	/* Ensure our globals are allocated. */
	if((err = allocate_globals()))
		goto f_error;

	/* Add the proc waiter to the list. */
	if(!ArrayList_add_tsafe(PROC_WAITER_CB_LIST, pw))
	{
		err = ALIB_MEM_ERR;
		goto f_error;
	}
	else
		pthread_cond_broadcast(PROC_WAITER_T_COND);

	return(ALIB_OK);
f_error:
	free(pw);
	return(err);
}
/* Registers a callback with the process waiter.
 *
 * Callbacks will be removed if their watched pid is greater than -1 and the received
 * pid from 'wait()' equals the watched pid.  If the watched pid is less than 0, then
 * the callback will never be removed from the event list unless manually removed by
 * calling 'proc_waiter_deregister()'.
 *
 * Parameters:
 * 		pid: The process ID returned by 'wait()' that should call 'proc_exited' callback.
 * 			If <0, the callback will be called for any PID that is returned by 'wait()'.
 * 		proc_exited: The callback called when 'wait()' returns a PID that matches 'pid'.
 * 		user_data: Data to be passed to the callback when called. */
alib_error proc_waiter_register(int pid, proc_exited_cb proc_exited, void* user_data)
{
	int err = proc_waiter_register_no_start(pid, proc_exited, user_data);
	if(err)return(err);

	return(proc_waiter_start());
}

/* Deregisters a callback previously registered with 'proc_waiter_register()'.
 *
 * The parameters will be used to search for a matching waiter and any registered
 * waiter that matches these parameters will be deregistered.  i.e. A call to
 * 'proc_waiter_deregister(0, NULL, NULL)' would remove all waiters registered
 * for a PID of 0.  A call to 'proc_waiter_deregister(1, my_proc, 0x11)' would
 * deregister all waiters registered to wait for PID 1 and call 'my_proc' with
 * memory located at 0x11.
 *
 * Parameters:
 * 		'pid': (OPTIONAL) The pid that was registered to be watched for.  If <0, then any callback
 * 			with attributes that match the other parameters will be deregistered.
 * 		'proc_exited': (OPTIONAL) The function pointer to the callback that should be removed.
 * 			If NULL, then any callback with attributes that match the other two parameters will
 * 			be deregistered.
 * 		'user_data': (OPTIONAL) Pointer to the data that would be passed to the callback.
 * 			If NULL, then any callback with attributes that match the other two parameters will
 * 			be deregistered. */
void proc_waiter_deregister(int pid, proc_exited_cb proc_exited, void* user_data)
{
	if(!PROC_WAITER_CB_LIST)return;

	/* Iterate through and deregister all process waiters. */
	proc_waiter** pw_it = (proc_waiter**)ArrayList_get_array_ptr(PROC_WAITER_CB_LIST);
	size_t pw_count;
	for(pw_count = 0; pw_count < ArrayList_get_count(PROC_WAITER_CB_LIST); ++pw_it)
	{
		if(*pw_it)
		{
			++pw_count;

			/* Remove the event from the event list. */
			if((pid < 0 || (*pw_it)->pid == pid) &&
					(!proc_exited || (*pw_it)->cb == proc_exited) &&
					(!user_data || (*pw_it)->user_data == user_data))
				ArrayList_remove_by_ptr_tsafe(PROC_WAITER_CB_LIST, (void**)pw_it);
		}
	}

	ArrayList_shrink_tsafe(PROC_WAITER_CB_LIST);
	pthread_cond_broadcast(PROC_WAITER_T_COND);
}
/* Calls 'proc_waiter_deregister' on all registered callbacks. */
void proc_waiter_deregister_all()
{
	if(!PROC_WAITER_CB_LIST)return;

	ArrayList_resize_tsafe(PROC_WAITER_CB_LIST, 0);
	pthread_cond_broadcast(PROC_WAITER_T_COND);
}

/* Stops the process waiter and frees all memory associated with it.
 *
 * If there are other threads that make calls on the process waiter
 * while 'free_proc_waiter()' is executing, behavior is undefined. */
void free_proc_waiter()
{
	/* Stop the waiter. */
	proc_waiter_stop_now();

	/* Free all data. */
	delArrayList(&PROC_WAITER_CB_LIST);
	if(PROC_WAITER_THREAD)
	{
		free(PROC_WAITER_THREAD);
		PROC_WAITER_THREAD = NULL;
	}
	if(PROC_WAITER_MUTEX)
	{
		pthread_mutex_destroy(PROC_WAITER_MUTEX);
		free(PROC_WAITER_MUTEX);
		PROC_WAITER_MUTEX = NULL;
	}
	if(PROC_WAITER_T_COND)
	{
		pthread_cond_destroy(PROC_WAITER_T_COND);
		free(PROC_WAITER_T_COND);
		PROC_WAITER_T_COND = NULL;
	}

	/* Reset all flags. */
	PROC_WAITER_FLAG_POLE = 0;
}

/*******Getters*******/
/* Returns !0 if the waiter thread is running.  0 otherwise. */
char proc_waiter_is_running(){return(PROC_WAITER_FLAG_POLE & THREAD_IS_RUNNING);}
/* Returns the number of microseconds the process waiter will sleep when
 * no child processes are connected to the current process. */
size_t proc_waiter_get_sleep_time(){return(PROC_WAITER_SLEEP_TIME);}
/*********************/

/*******Setters*******/
/* Sets the amount of time that the wait loop will wait if there are no
 * child processes.  Time is in microseconds.
 *
 * If the thread is already sleeping, it will finish sleeping for the original
 * duration.  On the next iteration, then the new sleep time will be used. */
void proc_waiter_set_sleep_time(size_t sleep_time){PROC_WAITER_SLEEP_TIME = sleep_time;}
/*********************/
