#ifndef SIGNAL_HANDLER_IS_DEFINED
#define SIGNAL_HANDLER_IS_DEFINED

#include <signal.h>

#include "alib_error.h"
#include "alib_cb_funcs.h"
#include "ArrayList.h"

/*******Function Callback Types*******/
/* The type of callback used by the signal handler.
 *
 * Parameters:
 * 		signum: The code for the signal received.
 * 		user_data: The user specified data that is related
 * 			to the callback. */
typedef void(*signal_handler_cb)(int signum, void* user_data);
/*************************************/

/*******Public Functions*******/
/* Adds a callback to the event list.
 *
 * Parameters:
 * 		signum: The signal that should fire the callback.
 * 		cb: The callback function that should be called.
 * 		user_data: This is the argument that will be passed to the callback.
 * 			Though allowed, it is suggested not to set the value to NULL unless
 * 			you never intend to deregister the callback. */
alib_error signal_handler_register(int signum, signal_handler_cb cb, void* user_data);

/* Deregisters all callbacks with a 'user_data' whose address matches the given
 * 'user_data' parameter address.
 *
 * Parameters:
 * 		signum: The signal to listen for.
 * 		cb: A pointer to the callback that is registered.
 * 			If null, then any registered callback that matches the other parameters
 * 			will be removed.
 * 		user_data: The user data that should be passed to the signal handler callback.
 * 			If null, then any registered callback that matches the other parameters
 * 			will be removed.
 */
void signal_handler_deregister(int signum, signal_handler_cb cb, void* user_data);

/* Calls 'signal_handler_deregister()' on all callbacks for all signals. */
void signal_handler_deregister_all();
/******************************/

#endif
