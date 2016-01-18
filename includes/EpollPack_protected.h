#ifndef EPOLL_PACK_PROTECTED_IS_DEFINED
#define EPOLL_PACK_PROTECTED_IS_DEFINED

#include "EpollPack.h"

/*******Protected Functions*******/
	/* Getters */
/* Returns a pointer to the object's mutex.
 *
 * This should be handled with care and the user MUST NEVER call pthread_mutex_destroy() on
 * the mutex.
 *
 * Assumes 'epp' is not null. */
pthread_mutex_t* EpollPack_get_mutex(EpollPack* epp);
	/***********/
/*********************************/

#endif
