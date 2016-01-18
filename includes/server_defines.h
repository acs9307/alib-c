#ifndef SERVER_DEFINES_ARE_DEFINED
#define SERVER_DEFINES_ARE_DEFINED

/*******Defines*******/
#ifndef DEFAULT_QUIT_COMMAND
#define DEFAULT_QUIT_COMMAND ".quit"
#endif

#ifndef DEFAULT_BACKLOG_SIZE
#define DEFAULT_BACKLOG_SIZE 256
#endif

#ifndef DEFAULT_INPUT_BUFF
#define DEFAULT_INPUT_BUFF_SIZE 64*1024
#endif
/*********************/

/*******Enums*******/
/* All values are should be thought of suggestions to the server
 * as it is never guaranteed that the server will do what is requested.
 * This is by design as file descriptors may be closed without notifications
 * or some internal error may occur due to object modification from another
 * thread. */
typedef enum server_cb_rval
{
	/* Run the callback in default mode. */
	SCB_RVAL_DEFAULT = 0,
	/* Request that the client be closed. */
	SCB_RVAL_CLOSE_CLIENT = 1,
	/* Request that the server be closed. */
	SCB_RVAL_STOP_SERVER = 2,
	/* Notifies the server that the request was
	 * completely handled by the user and that there
	 * is nothing else that needs to be done. */
	SCB_RVAL_HANDLED = 4,
	/* Requests that the server does not shut down or
	 * close a client, only useful when the default behavior
	 * is to close the client or shutdown the server. */
	SCB_RVAL_CONTINUE = 8,
	/* Requests that the server be safely cleaned up. */
	SCB_RVAL_DELETE = 16
}server_cb_rval;
/*******************/


#endif
