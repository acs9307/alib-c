#include "includes/signal_handler.h"

#include "ArrayList_protected.h"

/*******Private Structs*******/
/* Stores callback data. */
typedef struct signal_data
{
	signal_handler_cb cb;
	void* user_data;
}signal_data;

/* Handles calling all the callbacks for a single incoming signal. */
typedef struct signal_handler
{
	/* The signal listened for. */
	int sig;
	/* Callbacks for the given signal.
	 * Should be of type 'signal_data'. */
	ArrayList* sd_list;

	/* Struct for registering the signal handler proc. */
	struct sigaction sa;
}signal_handler;

/* Frees a signal handler. */
static void free_signal_handler(void* v_signal_handler)
{
	signal_handler* sh = (signal_handler*)v_signal_handler;

	delArrayList(&sh->sd_list);

	free(sh);
}
/*****************************/

/* List of signal handler structs.
 * Each signal handler should handle a unique signal. */
ArrayList* SIGNAL_HANDLER_LIST = NULL;

/*******Private Functions*******/
/* Function that is called whenever a signal is received from the system. */
static void signal_handler_proc(int signum)
{
	if(!SIGNAL_HANDLER_LIST)return;

	signal_data** cbs;
	size_t cbs_count;

	/* Ensure we are able to find a signal handler that can handle the given signal.
	 * We may not find one if a signal handler was removed after registering it. */
	signal_handler* sh = (signal_handler*)ArrayList_find_item_by_value_tsafe(
			SIGNAL_HANDLER_LIST, &signum, compare_int_ptr);
	if(!sh || !sh->sd_list)return;

	/* Iterate through and call each signal event callback. */
	cbs = (signal_data**)ArrayList_get_array_ptr(sh->sd_list);
	for(cbs_count = 0; cbs_count < ArrayList_get_count(sh->sd_list); ++cbs)
	{
		if(*cbs)
		{
			++cbs_count;
			if((*cbs)->cb)
				(*cbs)->cb(signum, (*cbs)->user_data);
		}
	}
}
/*******************************/

/*******Public Functions*******/
/* Adds a callback to the event list.
 *
 * Parameters:
 * 		signum: The signal that should fire the callback.
 * 		cb: The callback function that should be called.
 * 		user_data: This is the argument that will be passed to the callback.
 * 			Though allowed, it is suggested not to set the value to NULL unless
 * 			you never intend to deregister the callback. */
alib_error signal_handler_register(int signum, signal_handler_cb cb, void* user_data)
{
	signal_handler* handler;
	int err;

	signal_data* sd = malloc(sizeof(signal_data));
	if(!sd)return(ALIB_MEM_ERR);

	sd->cb = cb;
	sd->user_data = user_data;

	/* Find the handler by the signal number. */
	if(SIGNAL_HANDLER_LIST)
		handler = (signal_handler*)ArrayList_find_item_by_value_tsafe(
				SIGNAL_HANDLER_LIST, &signum, compare_int_ptr);
	/* SIGNAL_HANDLER_LIST has not been initialized yet, construct it. */
	else
	{
		SIGNAL_HANDLER_LIST = newArrayList(free_signal_handler);
		if(!SIGNAL_HANDLER_LIST)
		{
			err = ALIB_MEM_ERR;
			goto f_error;
		}
		handler = NULL;
	}

	/* If no handler was found, then we need to create a new handler. */
	if(!handler)
	{
		handler = malloc(sizeof(signal_handler));
		if(!handler)
		{
			err = ALIB_MEM_ERR;
			goto f_error;
		}
		handler->sig = signum;
		handler->sd_list = newArrayList(free);

		/* Register the signal handler. */
		handler->sa.sa_handler = signal_handler_proc;
		if(sigaction(handler->sig, &handler->sa, NULL))
		{
			err = ALIB_UNKNOWN_ERR;
			free_signal_handler(handler);
			goto f_error;
		}

		ArrayList_add_tsafe(SIGNAL_HANDLER_LIST, handler);
	}

	/* Ensure we have an 'sd_list'. */
	if(!handler->sd_list)
	{
		handler->sd_list = newArrayList(free);
		if(!handler->sd_list)
		{
			err = ALIB_MEM_ERR;
			goto f_error;
		}
	}

	/* If a handler was found, then we simply add the signal_data to the
	 * handler's array list. */
	if(!ArrayList_add_tsafe(handler->sd_list, sd))
	{
		err = ALIB_MEM_ERR;
		goto f_error;
	}

	return(ALIB_OK);

f_error:
	if(sd)free(sd);

	return(err);
}

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
void signal_handler_deregister(int signum, signal_handler_cb cb, void* user_data)
{
	/* Check arguments. */
	if(!SIGNAL_HANDLER_LIST)return;

	signal_data** list_it;
	size_t list_it_count = 0;

	/* Get the related handler. */
	signal_handler* handler = (signal_handler*)ArrayList_find_item_by_value_tsafe(
			SIGNAL_HANDLER_LIST, &signum, compare_int_ptr);
	if(!handler || !handler->sd_list)return;

	/* Iterate through the array list and remove all matching signal data objects. */
	list_it = (signal_data**)ArrayList_get_array_ptr(handler->sd_list);
	if(!list_it)return;
	for(; list_it_count < ArrayList_get_count(handler->sd_list);++list_it)
	{
		if(*list_it)
		{
			++list_it_count;
			if(((*list_it)->user_data == user_data || !user_data) &&
					((*list_it)->cb == cb || !cb))
				ArrayList_remove_by_ptr_tsafe(handler->sd_list, (void**)list_it);
		}
	}

	/* Resize the ArrayList if we removed anything. */
	if(ArrayList_get_count(SIGNAL_HANDLER_LIST) != ArrayList_get_capacity(SIGNAL_HANDLER_LIST))
		ArrayList_shrink_tsafe(handler->sd_list);
}

/* Calls 'signal_handler_deregister()' on all callbacks for all signals. */
void signal_handler_deregister_all()
{
	if(!SIGNAL_HANDLER_LIST)return;

	signal_handler** handler = (signal_handler**)ArrayList_get_array_ptr(SIGNAL_HANDLER_LIST);
	size_t handler_it_count;

	/* Delete each handler's callback list. */
	for(handler_it_count = 0; handler_it_count < ArrayList_get_count(SIGNAL_HANDLER_LIST);
			++handler_it_count, ++handler)
	{
		if((*handler))
			delArrayList(&(*handler)->sd_list);
	}
}
/******************************/
