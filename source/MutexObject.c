#include "includes/MutexObject.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>

/*******Private Functions*******/
/*Compares the two request numbers, returns 0 if the are equal.*/
char request_compare(void* i1, void* i2)
{
	if(*((unsigned int*)i1) == *((unsigned int*)i2))
		return(0);
	else
		return(1);
}
/*******************************/

/*******Lifecycle*******/
/* Creates a new MutexObject with
 * members set to default values.*/
MutexObject* newMutexObject()
{
	MutexObject* n_mutex = malloc(sizeof(MutexObject));
	if(!n_mutex)return(NULL);

	//Setup settings
	n_mutex->sleep_micros = 1000;
	n_mutex->timeout_millis = 5000;
	n_mutex->refuse_requests = 0;

	//Setup request handler list.
	n_mutex->request_num = 0;
	if(newListHistory(&n_mutex->request_list, -1))
	{
		free(n_mutex);
		return(NULL);
	}

	return(n_mutex);
}
/* Deletes a given mutex object and sets the pointer to null.*/
void delMutexObject(MutexObject** mutex)
{
	char err;
	MutexObject* t_ptr;

	if(mutex == NULL && *mutex == NULL)
		return;

	/* Attempt to get a lock, don't worry if it fails.
	 * If it has already been canceled, or the mutex is already
	 * been freed, just return. */
	err = MutexObject_request_lock(*mutex);
	if(err == MUTEX_NULL_ARG || err == MUTEX_CANCELED)
	{
		*mutex = NULL;
		return;
	}

	//We are destroying the object, we can't accept any more requests for locking.
	(*mutex)->refuse_requests = 1;

	t_ptr = *mutex;
	*mutex = NULL;
	if((*mutex)->request_list->count > 1)
	{
		/* There are other mutexes, try to force them to timeout
		 * nicely.*/
		t_ptr->timeout_millis = 1;
		usleep(10 * 1000); //Sleep for 10 milliseconds to ensure they will timeout.
	}

	delListHistory(&t_ptr->request_list);
	free(t_ptr);
}
/***********************/

/*******Functions*******/
/* Requests a lock on a mutex.
 *
 * Return Codes:
 * 		MUTEX_SUCCESS (0): Successfully locked within the timeout.
 * 		MUTEX_NULL_ARG (-1): Called with null argument.
 * 		MUTEX_TIMEOUT (-2): Unable to lock within the given timeout.
 * 		MUTEX_ERROR (-3): Error occured while searching for the time.*/
char MutexObject_request_lock(MutexObject* mutex)
{
	/* The number of this request. request_num should not be zero, causes bug with request_compare()*/
	unsigned int my_num = (++mutex->request_num)?mutex->request_num:++mutex->request_num;

	//This stores the number of times we will loop before returning a timeout code.
	int timeout_ticks = -1;
	unsigned int last_tick_sleep;

	//If we need to calculate time, we will use these pointers.
	struct timeval timeout_time;
	struct timeval cur_time;

	if(!mutex)
		return(MUTEX_NULL_ARG);

	/*Object is being destroyed, just cancel.*/
	if(mutex->refuse_requests)
		return(MUTEX_CANCELED);

	//Add request to list.
	mutex->request_list->Push_Back(mutex->request_list, &my_num, NULL, request_compare);

	if(mutex && mutex->timeout_millis)
	{
		if(mutex->sleep_micros)
		{
			while(mutex && mutex->request_list->list_root &&
					request_compare(mutex->request_list->list_root->list_item, &my_num) != 0)
			{
				//Calculate the timeout_ticks if we actually hit the loop.
				if(timeout_ticks < 0)
				{
					//Temporarily store a calculated number, we will need it twice later
					last_tick_sleep = mutex->sleep_micros / 1000;
					timeout_ticks = mutex->timeout_millis / last_tick_sleep;
					last_tick_sleep = mutex->timeout_millis % last_tick_sleep;
				}
				//Through an error
				else if(timeout_ticks == 0)
				{
					mutex->request_list->Remove(mutex->request_list, &my_num);
					return(MUTEX_TIMEOUT);
				}
				//Continue on, first decrement timeout_ticks
				else
					--timeout_ticks;

				/* If this is the last iteration and there was a calculated remainder in sleep time
				 * then we want to only sleep for the remainder of the timeout.*/
				if(timeout_ticks == 0 && last_tick_sleep != 0)
					usleep(last_tick_sleep * 1000);
				else
					usleep(mutex->sleep_micros);
			}
		}
		else
		{
			if(gettimeofday(&timeout_time, NULL))
			{
				mutex->request_list->Remove(mutex->request_list, &my_num);
				return(MUTEX_ERROR);
			}

			timeout_time.tv_sec += mutex->timeout_millis / 1000;
			timeout_time.tv_usec += (mutex->timeout_millis % 1000) * 1000;

			//Wrap the micro seconds if needed.
			if(timeout_time.tv_usec > 1000 * 1000)
			{
				++timeout_time.tv_sec;
				timeout_time.tv_usec -= 1000 * 1000;
			}

			/* For calculating micro second differences later, we need
			 * to wrap it now if it is getting closer.  May increase desired
			 * timeout time by one millisecond, however, should seldom be important.
			 */
			if(timeout_time.tv_usec > 990 * 1000)
			{
				++timeout_time.tv_sec;
				timeout_time.tv_usec = 0;
			}

			while(mutex && mutex->request_list->list_root &&
					request_compare(mutex->request_list->list_root->list_item, &my_num) != 0)
			{
				if((gettimeofday(&cur_time, NULL) == 0 && (cur_time.tv_sec < timeout_time.tv_sec ||
						cur_time.tv_sec > timeout_time.tv_sec + 5)) && ((cur_time.tv_sec > timeout_time.tv_sec &&
						(cur_time.tv_sec < timeout_time.tv_sec + 2  || timeout_time.tv_sec > timeout_time.tv_sec + 2) &&
						cur_time.tv_usec < timeout_time.tv_usec)))
				{
					mutex->request_list->Remove(mutex->request_list, &my_num);
					return(MUTEX_TIMEOUT);
				}
			}
		}
	}
	else
	{
		if(mutex->sleep_micros)
			while(mutex && mutex->request_list->list_root &&
					request_compare(mutex->request_list->list_root->list_item, &my_num) != 0)
				usleep(mutex->sleep_micros);
		else
			while(mutex && mutex->request_list->list_root &&
					request_compare(mutex->request_list->list_root->list_item, &my_num) != 0);
	}

	if(!mutex)
		return(MUTEX_CANCELED);

	//Successfully locked
	return(MUTEX_SUCCESS);
}

/* Unlocks a mutex
 *
 * Return Codes:
 * 		MUTEX_SUCCESS (0): Completed successfully.
 * 		MUTEX_NULL_ARG (-1): Called with null mutex pointer.*/
char MutexObject_unlock(MutexObject* mutex)
{
	if(!mutex)
		return(MUTEX_NULL_ARG);

	mutex->request_list->Pop_Off(mutex->request_list);
	return(MUTEX_SUCCESS);
}
/***********************/
