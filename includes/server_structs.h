#ifndef SERVER_STRUCTS_ARE_DEFINED
#define SERVER_STRUCTS_ARE_DEFINED

#include <stdlib.h>
#include <unistd.h>

#include "alib_types.h"

/*******Client Package*******/
/* A simple struct to hold data related to a specific socket. */
typedef struct socket_package
{
	const int sock;
	void* user_data;
	alib_free_value free_user_data;

	/* Parent of the package, this will usually point to the
	 * server. */
	void* parent;
}socket_package;

	/* Constructors */
/* Allocates a new socket package struct and
 * sets its values. */
socket_package* new_socket_package(int sock);
	/****************/

	/* Destructors */
/* Frees a socket package. */
void free_socket_package(socket_package* package);
/* Frees a socket package and sets the pointer to NULL. */
void del_socket_package(socket_package** package);

/* Closes the client's socket (if it is not below 0) and frees
 * the package. */
void close_and_free_socket_package(socket_package* package);
/* Closes the client's socket (if it is not below 0) and frees
 * the package then sets the 'package' pointer to null. */
void close_and_del_socket_package(socket_package** package);
	/***************/

	/* Destruction Callbacks */
/* Callback to free the socket package.  This is  'alib_free_value'
 * compliant. */
void free_socket_package_cb(void* package);
/* Callback to delete the socket package.  This is 'alib_del_obj'
 * compliant. */
void del_socket_package_cb(void** package);

/* Callback which calls 'close_and_free_socket_package()'.  This is
 * 'alib_free_value' compliant. */
void close_and_free_socket_package_cb(void* package);
/* Callback which calls 'close_and_del_socket_package()'.  This is
 * 'alib_del_obj' compliant. */
void close_and_del_socket_package_cb(void** package);
	/*************************/
/****************************/



#endif
