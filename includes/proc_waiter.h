#ifndef PROC_WAITER_IS_DEFINED
#define PROC_WAITER_IS_DEFINED

#include <pthread.h>
#include <sys/wait.h>
#include <unistd.h>

#include "alib_time.h"
#include "alib_error.h"
#include "ArrayList.h"
#include "flags.h"

typedef void (*proc_exited_cb)(int pid, int status, void* user_data);

/* Calls for the process waiter to stop, but it will
 * continue to run until at least one process exits. */
void proc_waiter_stop();
/* Calls for the waiting thread to stop and then waits
 * for it to return.
 *
 * WILL BLOCK.*/
void proc_waiter_stop_wait();
/* Forces the process waiter to stop immediately.
 * It is suggested to use 'proc_waiter_stop()' or
 * 'proc_waiter_stop_wait()' instead as this will create
 * a new process that immediately exits so that the call to
 * 'wait()' will no longer block. */
void proc_waiter_stop_now();

/* Starts the thread.  If the thread is already running
 * 'ALIB_OK' will be returned.
 *
 * Waiting for child processes to close is handled on a separate thread. */
alib_error proc_waiter_start();

/* Broadcasts on the waiter's condition and wakes up any related waiting threads.
 *
 * Use after you have forked a new process. */
void proc_waiter_wakeup();

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
alib_error proc_waiter_register_no_start(int pid, proc_exited_cb proc_exited, void* user_data);
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
alib_error proc_waiter_register(int pid, proc_exited_cb proc_exited, void* user_data);

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
void proc_waiter_deregister(int pid, proc_exited_cb proc_exited, void* user_data);
/* Calls 'proc_waiter_deregister' on all registered callbacks. */
void proc_waiter_deregister_all();

/* Stops the process waiter and frees all memory associated with it.
 *
 * If there are other threads that make calls on the process waiter
 * while 'free_proc_waiter()' is executing, behavior is undefined. */
void free_proc_waiter();

/*******Thread Safe Functions*******/
/* Thread safe version of 'proc_waiter_register_no_start()'.
 *
 * Locks the ArrayList mutex. */
alib_error proc_waiter_register_no_start_tsafe(int pid, proc_exited_cb proc_exited, void* user_data);
/* Thread safe version of 'proc_waiter_register()'.
 *
 * Locks the ArrayList mutex. */
alib_error proc_waiter_register_tsafe(int pid, proc_exited_cb proc_exited, void* user_data);
/* Thread safe version of 'proc_waiter_deregister()'.
 *
 * Locks the ArrayList mutex. */
void proc_waiter_deregister_tsafe(int pid, proc_exited_cb proc_exited, void* user_data);
/* Thread safe version of 'proc_waiter_deregister_all()'.
 *
 * Locks the ArrayList mutex. */
void proc_waiter_deregister_all_tsafe();
/***********************************/

/*******Getters*******/
/* Returns !0 if the waiter thread is running.  0 otherwise. */
char proc_waiter_is_running();
/* Returns the number of microseconds the process waiter will sleep when
 * no child processes are connected to the current process. */
int64_t proc_waiter_get_sleep_time();
/* Returns true if the process waiter's globals have been initialized. */
char proc_waiter_is_initialized();
/*********************/

/*******Setters*******/
/* Sets the amount of time that the wait loop will wait if there are no
 * child processes.  Time is in microseconds.
 *
 * If the thread is already sleeping, it will finish sleeping for the original
 * duration.  On the next iteration, then the new sleep time will be used. */
void proc_waiter_set_sleep_time(int64_t sleep_time);
/*********************/

#endif
