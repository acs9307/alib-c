#ifndef MUTEX_OBJECT_IS_DEFINED
#define MUTEX_OBJECT_IS_DEFINED

#include "ListHistory.h"

/* NOTE:
 * 		This header contains functions which are only available on unix systems,
 * 			these functions are the 'usleep()'*/

/*TODO:
 * 		ADD: Support for Windows.*/

/*Defines*/
enum
{
	MUTEX_TIMEOUT = -101,
	MUTEX_CANCELED = -100,
	MUTEX_ERROR = ALIB_MUTEX_ERR,
	MUTEX_NULL_ARG = ALIB_BAD_ARG,
	MUTEX_SUCCESS = ALIB_OK
};
/*Mutex operation succeeded.*/
#define MUTEX_SUCCESS 0
/*Mutex operation called with null pointer.*/
#define MUTEX_NULL_ARG -1
/*Mutex operation timed out.*/
#define MUTEX_TIMEOUT -2
/*Mutex operation failed for unknown reason.*/
#define MUTEX_ERROR -3
/* Mutex operation was caceled, usually because the object
 * is being destroyed.*/
#define MUTEX_CANCELED -4
/*********/

typedef struct MutexObject
{
	/* The time to wait until throwing an exception.
	 *
	 * Time in milliseconds to wait before returning
	 * an error.  If the value is set to 0, then
	 * no timeout will be used and will wait forever.
	 * Default value: 5 seconds.*/
	unsigned int timeout_millis;
	/* If the thread has to wait, then sleep for this many
	 * milliseconds.  It is advised to keep this small as
	 * other threads in the queue will also be waiting.
	 *
	 * Default Value: 1 millisecond.*/
	unsigned long sleep_micros;
	/* Shows whether or not to accept new requests, should only
	 * be marked true if object cannot currently accept new requests.*/
	char refuse_requests;

	/*The number of the last request made.*/
	unsigned int request_num;
	/* List of requests made.
	 * Requests will be unique by the ListHistoryItem that it points to.*/
	struct ListHistory* request_list;
}MutexObject;

/*******Lifecycle*******/
/* Creates a new MutexObject with
 * members set to default values.*/
MutexObject* newMutexObject();
/* Deletes a given mutex object and sets the pointer to null.*/
void delMutexObject(MutexObject**);
/***********************/

/*******Functions*******/
/* Requests a lock on a mutex.
 *
 * Return Codes:
 * 		MUTEX_SUCCESS (0): Successfully locked within the timeout.
 * 		MUTEX_NULL_ARG (-1): Called with null argument.
 * 		MUTEX_TIMEOUT (-2): Unable to lock within the given timeout.
 * 		MUTEX_ERROR (-3): Error occured while searching for the time.*/
char MutexObject_request_lock(MutexObject* mutex);

/* Unlocks a mutex
 *
 * Return Codes:
 * 		MUTEX_SUCCESS (0): Completed successfully.
 * 		MUTEX_NULL_ARG (-1): Called with null mutex pointer.*/
char MutexObject_unlock(MutexObject* mutex);
/***********************/

#endif
