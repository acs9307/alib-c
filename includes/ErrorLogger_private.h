#ifndef ERROR_LOGGER_PRIVATE_IS_DEFINED
#define ERROR_LOGGER_PRIVATE_IS_DEFINED

#include "ErrorLogger_protected.h"

struct ErrorLogger
{
	/* The file path of the file to log to. */
	char* file_path;
	/* The name of the app that object belongs to. */
	char* app_name;

	/* The option used for timing. */
	const ErrorLoggerTimeOption time_option;
	/* The time that the object was created.  This will only be set
	 * if 'time_option' is set to ELTO_RUN_TIME. */
	const time_t startup_time;
	/* The number of times the logger has logged an error. */
	size_t log_count;

	/* The file pointer used by the object to write data to.
	 * If this is not null, then logging should wait until it is available again.
	 * This is required for multithreaded applications. */
	FILE* file;
	/* The time to wait before returning an error when trying to access the file member.
	 * The time is in milliseconds.  If set to -1, if an infinite timeout is desired.
	 * Defaults to 500 milliseconds. */
	int timeout;
	/* Mutex for handling the file used for logging. */
	pthread_mutex_t mutex;
	/* The maximum size of the file allowed to be made in bytes. */
	size_t max_file_size;

	el_truncate_cb truncate_cb;
	el_truncate_ready_cb truncate_ready_cb;
};

#endif
