#include "EpollPack_private.h"

/*******Protected Functions*******/
	/* Getters */
/* Returns a pointer to the object's mutex.
 *
 * This should be handled with care and the user MUST NEVER call pthread_mutex_destroy() on
 * the mutex.
 *
 * Assumes 'epp' is not null. */
pthread_mutex_t* EpollPack_get_mutex(EpollPack* epp){return(&epp->mutex);}
	/***********/
/*********************************/

/*******Public Functions*******/
/* Adds a socket to the list of sockets to listen for events on to the EpollPack.
 *
 * Parameters:
 * 		ep: The object to modify.
 * 		sock: The socket to start listening on.
 *
 * Returns the return value of 'epoll_ctl()'.  */
int EpollPack_add_sock(EpollPack* ep, uint32_t event_type, int sock)
{
	int err;

	if(!ep || sock < 0)return(ALIB_BAD_ARG);

	pthread_mutex_lock(&ep->mutex);
	ep->mod_event.data.fd = sock;
	ep->mod_event.events = event_type;
	err = epoll_ctl(ep->efd, EPOLL_CTL_ADD, sock, &ep->mod_event);
	pthread_mutex_unlock(&ep->mutex);

	return(err);
}

	/* Mutexing */
/* Locks the mutex for the object.
 *
 * Assumes 'epp' is not null. */
int EpollPack_lock(EpollPack* epp){return(pthread_mutex_lock(&epp->mutex));}
/* Unlocks the mutex for the object.
 *
 * Assumes 'epp' is not null. */
int EpollPack_unlock(EpollPack* epp){return(pthread_mutex_unlock(&epp->mutex));}
	/************/

	/* Getters */
/* Gets the EpollPack's file descriptor.
 *
 * Assumes that 'epp' is not null. */
int EpollPack_get_efd(EpollPack* epp){return(epp->efd);}
/* Gets the EpollPack's triggered events array.
 *
 * Assumes that 'epp' is not null. */
struct epoll_event* EpollPack_get_triggered_events(EpollPack* epp)
{
	return(epp->triggered_events);
}
/* Gets the EpollPack's number of triggered events that have been allocated.
 *
 * Assumes that 'epp' is not null. */
size_t EpollPack_get_triggered_event_len(EpollPack* epp)
{
	return(epp->te_len);
}
/* Gets the EpollPack's user data.
 *
 * Assumes that 'epp' is not null. */
void* EpollPack_get_user_data(EpollPack* epp){return(epp->user_data);}
/* Gets the EpollPack's mod event.  This is used for manually handling 'epoll_ctl()'
 * functions.
 *
 * Assumes 'epp' is not null. */
struct epoll_event* EpollPack_get_mod_event(EpollPack* epp){return(&epp->mod_event);}
	/***********/
/******************************/

/*******Lifecycle*******/
/* Creates a new epoll package object. Nothing else has to be done
 * to the object before it is used with epoll functions.
 *
 * Parameters:
 * 		event_count: (Optional) The maximum number of event to handle
 * 			at a given time. If set to 0, DEFAULT_BACKLOG_SIZE will be
 * 			used.
 * 		user_data: (Optional) Data associated with the epoll package.
 * 		free_user_data: (Optional) Called whenever the epoll package
 * 			is about to be destroyed.
 *
 * Returns:
 * 		EpollPack*: Success.
 * 		NULL: Error. */
EpollPack* newEpollPack(size_t event_count, void* user_data,
		alib_free_value free_user_data)
{
	EpollPack* epp = malloc(sizeof(EpollPack));
	if(!epp)return(NULL);

	/* If no event count is given, set to the default value. */
	if(!event_count)
		event_count = DEFAULT_BACKLOG_SIZE;

	/* Initialize epoll members. */
	epp->efd = epoll_create(event_count);
	epp->triggered_events = calloc(event_count, sizeof(struct epoll_event));
	epp->te_len = event_count;
	pthread_mutex_init(&epp->mutex, NULL);

	/* Set user data. */
	epp->user_data = user_data;
	epp->free_user_data = free_user_data;

	/* Check for errors. */
	if(!epp->triggered_events)
		delEpollPack(&epp);

	return(epp);
}

/* Destroys the EpollPack. */
void delEpollPack(EpollPack** epp)
{
	if(!epp || !*epp)return;

	/* Ensure that the object is locked before trying to close the file descriptor. */
	pthread_mutex_lock(&(*epp)->mutex);
	if((*epp)->efd)
	{
		close((*epp)->efd);
		(*epp)->efd = -1;
	}
	pthread_mutex_unlock(&(*epp)->mutex);

	/* Free the user data. */
	if((*epp)->free_user_data && (*epp)->user_data)
		(*epp)->free_user_data((*epp)->user_data);

	/* Free the triggered events. */
	if((*epp)->triggered_events)
		free((*epp)->triggered_events);

	/* Hopefully nothing bad has happened or will happen when
	 * we destroy the mutex. */
	pthread_mutex_destroy(&(*epp)->mutex);

	free(*epp);
	*epp = NULL;
}
/***********************/
