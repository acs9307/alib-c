#ifndef ALIB_ERROR_IS_INCLUDED
#define ALIB_ERROR_IS_INCLUDED

typedef enum alib_error
{
	ALIB_OVERFLOW = -22,
	ALIB_FILE_NOT_FOUND = -21,
	ALIB_FILE_FORMAT_ERR = -19,
	/* Operation timed out. */
	ALIB_TIMEOUT = -18,
	/* Given index is bad. */
	ALIB_BAD_INDEX = -17,
	/* An internal maximum flag was reached, usually referring to
	 * a maximum amount of memory allowed. */
	ALIB_INTERNAL_MAX_REACHED = -16,
	/* Error occurred with a thread. */
	ALIB_THREAD_ERR = -15,
	/* Error occurred while writing to a file/file descriptor. */
	ALIB_FILE_WRITE_ERR = -14,
	/* Error occurred while reading from a file/file descriptor. */
	ALIB_FILE_READ_ERR = -13,
	/* Could not open a particular file. */
	ALIB_FILE_OPEN_ERR = -12,
	/* Generic file error, some file operation failed
	 * on the file. */
	ALIB_FILE_ERR = -11,
	/* Error message is located in errno. */
	ALIB_CHECK_ERRNO = -10,
	/* Object has been corrupted.  Thought this
	 * may be returned, after returning it is not guaranteed to
	 * still be corrupted as the object may have been able
	 * to fix itself. */
	ALIB_OBJ_CORRUPTION = -9,
	/* Error occurred during a recv() operation. */
	ALIB_TCP_RECV_ERR = -20,
	/* Unable to send over TCP. */
	ALIB_TCP_SEND_ERR = -8,
	/* Unable to connect via TCP. */
	ALIB_TCP_CONNECT_ERR = -7,
	/* Error occurred on a file descriptor. */
	ALIB_FD_ERROR = -6,
	/* Unknown error. */
	ALIB_UNKNOWN_ERR = -5,
	/* Failed to bind to the port as it is
	 * already in use. */
	ALIB_PORT_IN_USE = -4,
	/* Error occurred during locking or
	 * unlocking of mutex. */
	ALIB_MUTEX_ERR = -3,
	/* Could not allocate memory. */
	ALIB_MEM_ERR = -2,
	/* An invalid argument was passed to a function. */
	ALIB_BAD_ARG = -1,
	/* No errors. */
	ALIB_OK = 0
}alib_error;

#endif
