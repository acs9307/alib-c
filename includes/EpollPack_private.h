#ifndef EPOLL_PACK_PRIVATE_IS_DEFINED
#define EPOLL_PACK_PRIVATE_IS_DEFINED

#include "EpollPack.h"

/* Container object used to store members related to an epoll socket.
 * The object is fully thread safe. */
struct EpollPack
{
	/* Epoll file descriptor. */
	int efd;
	/* Array of events that have been triggered. */
	struct epoll_event* triggered_events;
	/* The number of events triggered. */
	size_t te_len;
	/* Used to add or modify an event handler. */
	struct epoll_event mod_event;

	/* User data members. */
	void* user_data;
	alib_free_value free_user_data;

	/* Mutex for thread safety. */
	pthread_mutex_t mutex;
};

#endif
