#include "server_structs.h"


/*******Client Package*******/
	/* Constructors */
/* Allocates a new socket package struct and
 * sets its values. */
socket_package* new_socket_package(int sock)
{
	socket_package* package = malloc(sizeof(socket_package));
	if(!package)return(NULL);

	*((int*)&package->sock) = sock;
	package->user_data = NULL;
	package->free_user_data = NULL;
	package->parent = NULL;

	return(package);
}
	/****************/

	/* Destructors */
/* Frees a socket package. */
void free_socket_package(socket_package* package)
{
	if(!package)return;

	if(package->free_user_data && package->user_data)
		package->free_user_data(package->user_data);
	free(package);
}
/* Frees a socket package and sets the pointer to NULL. */
void del_socket_package(socket_package** package)
{
	if(!package)return;

	free_socket_package(*package);
	*package = NULL;
}

/* Closes the client's socket (if it is not below 0) and frees
 * the package. */
void close_and_free_socket_package(socket_package* package)
{
	if(!package)return;

	if(package->sock > -1)
		close(package->sock);
	free_socket_package(package);
}
/* Closes the client's socket (if it is not below 0) and frees
 * the package then sets the 'package' pointer to null. */
void close_and_del_socket_package(socket_package** package)
{
	if(!package)return;

	close_and_free_socket_package(*package);
	*package = NULL;
}
	/***************/

	/* Destruction Callbacks */
/* Callback to free the socket package.  This is  'alib_free_value'
 * compliant. */
void free_socket_package_cb(void* package)
{
	free_socket_package((socket_package*)package);
}
/* Callback to delete the socket package.  This is 'alib_del_obj'
 * compliant. */
void del_socket_package_cb(void** package)
{
	del_socket_package((socket_package**)package);
}
/* Callback which calls 'close_and_free_socket_package()'.  This is
 * 'alib_free_value' compliant. */
void close_and_free_socket_package_cb(void* package)
{
	close_and_free_socket_package((socket_package*)package);
}
/* Callback which calls 'close_and_del_socket_package()'.  This is
 * 'alib_del_obj' compliant. */
void close_and_del_socket_package_cb(void** package)
{
	close_and_del_socket_package((socket_package**)package);
}
	/*************************/
/****************************/
