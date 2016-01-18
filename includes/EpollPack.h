#ifndef EPOLL_PACK_IS_DEFINED
#define EPOLL_PACK_IS_DEFINED

#include <sys/epoll.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>

#include "alib_types.h"
#include "alib_error.h"
#include "server_defines.h"

/* Container object used to store members related to an epoll socket.
 * The object is fully thread safe. */
typedef struct EpollPack EpollPack;

/*******Public Functions*******/
/* Adds a socket to the list of sockets to listen for events on to the EpollPack.
 *
 * Parameters:
 * 		ep: The object to modify.
 * 		event_type: The type of event to handle on this socket, usually just EPOLLIN.
 * 		sock: The socket to start listening on.
 *
 * Returns the return value of 'epoll_ctl()'.  */
int EpollPack_add_sock(EpollPack* ep, uint32_t event_type, int sock);

	/* Mutexing */
/* Locks the mutex for the object.
*
* Assumes 'epp' is not null. */
int EpollPack_lock(EpollPack* epp);
/* Unlocks the mutex for the object.
*
* Assumes 'epp' is not null. */
int EpollPack_unlock(EpollPack* epp);
	/************/

	/* Getters */
/* Gets the EpollPack's file descriptor.
 *
 * Assumes that 'epp' is not null. */
int EpollPack_get_efd(EpollPack* epp);
/* Gets the EpollPack's triggered events array.
 *
 * Assumes that 'epp' is not null. */
struct epoll_event* EpollPack_get_triggered_events(EpollPack* epp);
/* Gets the EpollPack's number of triggered events that have been allocated.
 *
 * Assumes that 'epp' is not null. */
size_t EpollPack_get_triggered_event_len(EpollPack* epp);
/* Gets the EpollPack's user data.
 *
 * Assumes that 'epp' is not null. */
void* EpollPack_get_user_data(EpollPack* epp);
/* Gets the EpollPack's mod event.  This is used for manually handling 'epoll_ctl()'
 * functions.
 *
 * Assumes 'epp' is not null. */
struct epoll_event* EpollPack_get_mod_event(EpollPack* epp);
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
		alib_free_value free_user_data);

/* Destroys the EpollPack. */
void delEpollPack(EpollPack** epp);
/***********************/

#endif
