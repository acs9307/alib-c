#ifndef ERROR_LOGGER_IS_DEFINED
#define ERROR_LOGGER_IS_DEFINED

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <json-c/json.h>

#include "alib_error.h"
#include "alib_file.h"
#include "alib_string.h"

/*******Defines*******/
#define DEFAULT_MAX_ELOGGER_SIZE 1024 * 1024	//1 Mb.
/*********************/

/* Options for logging time for the ErrorLogger. */
typedef enum ErrorLoggerTimeOptions
{
	/* If set, no time will be placed in the log file. */
	ELTO_NO_TIME = 0,
	/* If set, the time that has elapsed (in seconds) since
	 * the creation of the object will be placed in the log file. */
	ELTO_RUN_TIME = 1,
	/* If set, the system time will be placed
	 * in the log file. */
	ELTO_CURRENT_TIME = 2,
}ErrorLoggerTimeOption;

/* A class designed to help with logging errors simply, consistently, and safely. */
typedef struct ErrorLogger ErrorLogger;

/*******Callback Types*******/
/* Called whenever it is determined that the ErrorLogger file
 * must be truncated.
 *
 * Parameters:
 * 		logger: The logger who's file should be truncate.
 * 		size: The size of the of the file, in bytes.  For
 * 			consistency, it is suggested to calculate the
 * 			size if the argument is 0.
 */
typedef void(*el_truncate_cb)(ErrorLogger* logger, size_t size);
/* Called whenever the file needs to be checked to determine if
 * it can be truncated.
 *
 * All parameters are optional and all depend on the implementation
 * made by the user.  For consistency, if 'size' is 0, then the file
 * size should be calculated within the callback.
 *
 * Parameters:
 * 		logger: The logger object who's file must be checked.
 * 		size: The size of the file in bytes.
 *
 * Returns:
 * 		0: The file is not ready for truncation.
 * 		!0: The file is ready for truncation.
 */
typedef char(*el_truncate_ready_cb)(ErrorLogger* logger, size_t size);
/****************************/

/*******Callback Functions*******/
/* Truncates the size of the file by at least half.  Only complete
 * json objects will be removed from the file. */
void ErrorLogger_default_truncate_size(ErrorLogger* logger, size_t f_size);
/* Determines if the file has reached maximum size.
 *
 * Parameters:
 * 		logger: The logger whose file will be check for resizing.
 * 		f_size: The size of the file, in bytes.  If the value is 0, the size will be calculated
 * 			within the callback.
 */
char ErrorLogger_default_truncate_size_ready(ErrorLogger* logger, size_t f_size);
/********************************/

/*******Public Functions*******/
/* Logs error data to the ErrorLogger file.
 *
 * This function WILL block until either the timeout is reached or
 * the file is unlocked.
 *
 * Parameters:
 * 		logger: (REQUIRED) The logger to use for logging.
 * 		loc: (OPTIONAL) The location the error occurred in,
 * 			this should usually be the file name and function.
 * 		err_code: (OPTIONAL) The error code of the error that occurred.
 * 		err_msg: (OPTIONAL) The message associated with the error.
 */
alib_error ErrorLogger_log_error(ErrorLogger* logger, const char* loc, int err_code,
		const char* err_msg);

	/* Getters */
/* Gets the file path that the ErrorLogger will log data to.
 *
 * Assumes 'logger' is not null. */
const char* ErrorLogger_get_file_path(ErrorLogger* logger);
/* Gets the application name that the ErrorLogger will use in logging.
 *
 * Assumes 'logger' is not null. */
const char* ErrorLogger_get_app_name(ErrorLogger* logger);
/* Gets the time option that the ErrorLogger will use in logging.
 *
 * Assumes 'logger' is not null. */
ErrorLoggerTimeOption ErrorLogger_get_time_option(ErrorLogger* logger);
/* Gets the time that the ErrorLogger was created.
 *
 * Assumes 'logger' is not null. */
time_t ErrorLogger_get_startup_time(ErrorLogger* logger);
/* Gets the number of times the ErrorLogger has logged an error.
 * If more than SIZE_MAX errors have been logged, then the value
 * will be inaccurate.
 *
 * Assumes 'logger' is not null. */
size_t ErrorLogger_get_log_count(ErrorLogger* logger);
/* Gets the timeout of the ErrorLogger. This is the time used
 * to wait for the mutex to be locked before failing.
 *
 * Assumes 'logger' is not null. */
int ErrorLogger_get_timeout(ErrorLogger* logger);
/* Gets the maximum size a file can be (in bytes) before
 * it is truncated.
 *
 * Assumes 'logger' is not null. */
size_t ErrorLogger_get_max_file_size(ErrorLogger* logger);
	/***********/

	/* Setters */
/* Sets the file path to the file that should be used for logging errors. */
void ErrorLogger_set_file_path(ErrorLogger* logger, const char* file_path);
/* Sets the application name that will be used when logging errors. */
void ErrorLogger_set_app_name(ErrorLogger* logger, const char* app_name);
/* Sets the logger's time option, this will determine what time data will be
 * written to the file upon error.
 *
 * Assumes 'logger' is not null. */
void ErrorLogger_set_time_option(ErrorLogger* logger, ErrorLoggerTimeOption option);
/* Sets the timeout of the ErrorLogger.  This is the time used
 * to wait for the mutex to be locked before failing.  If
 * set to <0, then write operations will block forever.
 *
 * Parameters:
 * 		logger: The object to modify.
 * 		timeout: The time in milliseconds for write operations to wait for
 * 			control of file pointer.
 *
 * Assumes 'logger' is not null. */
void ErrorLogger_set_timeout(ErrorLogger* logger, int timeout);
/* Sets the maximum number of bytes a file can use before it is truncated.
 *
 * Assumes 'logger' is not null. */
void ErrorLogger_set_max_file_size(ErrorLogger* logger, size_t max_file_size);

/* Sets the truncation algorithms to use the default callbacks based on size.
 * This decreases the size of the file by at least half by removing the
 * oldest logs first.  Max size is determined by DEFAULT_MAX_ELOGGER_SIZE.
 *
 * Assumes 'logger' is not null. */
void ErrorLogger_use_default_truncate_size_cb(ErrorLogger* logger);
/* Sets the truncation callback for the ErrorLogger.  This will be called
 * whenever the logger's 'truncate_ready_cb' returns true. If the function
 * is null, then the file will never be truncated.
 *
 * Assumes 'logger' is not null. */
void ErrorLogger_set_truncate_cb(ErrorLogger* logger, el_truncate_cb truncate_cb);
/* Sets the truncation ready callback for the ErrorLogger.  This will be called
 * whenever the ErrorLogger prepares to append data to the file.  If the function
 * is null, then the file will never be truncated.
 *
 * Assumes 'logger' is not null. */
void ErrorLogger_set_truncate_ready_cb(ErrorLogger* logger,
		el_truncate_ready_cb truncate_ready_cb);
	/***********/
/******************************/

/*******Constructors*******/
/* Creates a new ErrorLogger object.  All strings will be copied into the object's
 * internal memory and will be freed when the object is destroyed.
 *
 * Parameters:
 * 		file_path: (REQUIRED) The path to the file which will be used to write the
 * 			error data to.
 * 		time_option: (REQUIRED) The option used to determine which timing method to use
 * 			for headers of the error data written.  If not needed, use ELTO_NO_TIME.
 * 		app_name: (OPTIONAL) This should be the name of the application that is logging the
 * 			error.
 *
 * Returns:
 * 		NULL: Error.
 * 		ErrorLogger*: The newly instantiated ErrorLogger object. */
ErrorLogger* newErrorLogger(const char* file_path, ErrorLoggerTimeOption time_option,
		const char* app_name);

/* Destroys an ErrorLogger object and sets the pointer to NULL. */
void delErrorLogger(ErrorLogger** logger);
/**************************/

#endif
