#include "ErrorLogger_private.h"

/*******Callback Functions*******/
/* Truncates the size of the file by at least half.  Only complete
 * json objects will be removed from the file. */
void ErrorLogger_default_truncate_size(ErrorLogger* logger, size_t f_size)
{
	if(!logger || !logger->file_path)return;

	FILE* file;
	char* buff;
	char* it, *end_it;

	file = fopen(logger->file_path, "r");
	if(!file)return;

	/* Calculate the size if need be. */
	if(!f_size)
		f_size = file_size(file);
	if(f_size % 2)
	{
		f_size /= 2;
		fseek(file, f_size + 1, SEEK_SET);
	}
	else
	{
		f_size /= 2;
		fseek(file, f_size, SEEK_SET);
	}

	/* If the new size is 0, then we just reopen the file
	 * and close it to truncate the memory. */
	if(!f_size)
	{
		fclose(file);
		file = fopen(logger->file_path, "w+");
		fclose(file);
		return;
	}

	/* Allocate buffer memory. */
	buff = malloc(f_size);
	if(!buff)
		goto f_return;
	if(fread(buff, 1, f_size, file) != f_size)
		goto f_return;

	/* Find the end of the current section of data. */
	for(it = buff, end_it = buff + f_size; *it != '}' && *it != '{' && it != end_it; ++it);
	if(it == end_it)
		goto f_return;
	else if(*it == '}')
		it = find_next_non_whitespace(++it);

	/* Reopen the file and write to it. */
	freopen(logger->file_path, "w+", file);
	if(!file)goto f_return;
	fwrite(it, 1, (int)(end_it - it), file);

f_return:
	if(buff)
		free(buff);
	if(file)
		fclose(file);
}
/* Determines if the file has reached maximum size.
 *
 * Parameters:
 * 		logger: The logger whose file will be check for resizing.
 * 		f_size: The size of the file, in bytes.  If the value is 0, the size will be calculated
 * 			within the callback.
 */
char ErrorLogger_default_truncate_size_ready(ErrorLogger* logger, size_t f_size)
{
	if(!logger || !logger->file)return(0);

	if(f_size == 0)
		f_size = file_size(logger->file);
	if(f_size < logger->max_file_size)
		return(0);
	else
		return(1);
}
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
alib_error ErrorLogger_log_error(ErrorLogger* logger, const char* loc, int err_code, const char* err_msg)
{
	int timeout = logger->timeout;
	json_object* obj;

	if(!logger)return(ALIB_BAD_ARG);

	/* Try locking the object. */
	while(pthread_mutex_trylock(&logger->mutex))
	{
		usleep(1000);
		if(!(--timeout))
			break;
	}
	if(timeout == 0)
		return(ALIB_MUTEX_ERR);

	/* Open the file. */
	logger->file = fopen(logger->file_path, "a");
	if(!logger->file)return(ALIB_FILE_OPEN_ERR);

	obj = json_object_new_object();

	/* Write the time before the error, if a time option was set. */
	if(logger->time_option != ELTO_NO_TIME)
	{
		time_t now = time(NULL);
		if(logger->time_option & ELTO_RUN_TIME)
			json_object_object_add(obj, "runTime", json_object_new_int64(now - logger->startup_time));
		if(logger->time_option & ELTO_CURRENT_TIME)
			json_object_object_add(obj, "currentTime", json_object_new_string(ctime(&now)));
	}

	/* Write the app name. */
	if(logger->app_name)
		json_object_object_add(obj, "appName", json_object_new_string(logger->app_name));

	/* Write the location to the file. */
	if(loc)
		json_object_object_add(obj, "location", json_object_new_string(loc));

	/* Print the error data. */
	if(err_msg)
		json_object_object_add(obj, "errMsg", json_object_new_string(err_msg));

	/* Check to see if the file must be resized. */
	if(logger->truncate_ready_cb(logger, 0))
		logger->truncate_cb(logger, 0);

	/* Write the data to the file. */
	fputs(json_object_to_json_string(obj), logger->file);
	fputc('\n', logger->file);
	fflush(logger->file);
	fclose(logger->file);
	logger->file = NULL;

	++logger->log_count;
	pthread_mutex_unlock(&logger->mutex);
	return(ALIB_OK);
}

	/* Getters */
/* Gets the file path that the ErrorLogger will log data to.
 *
 * Assumes 'logger' is not null. */
const char* ErrorLogger_get_file_path(ErrorLogger* logger)
{
	return(logger->file_path);
}
/* Gets the application name that the ErrorLogger will use in logging.
 *
 * Assumes 'logger' is not null. */
const char* ErrorLogger_get_app_name(ErrorLogger* logger)
{
	return(logger->app_name);
}
/* Gets the time option that the ErrorLogger will use in logging.
 *
 * Assumes 'logger' is not null. */
ErrorLoggerTimeOption ErrorLogger_get_time_option(ErrorLogger* logger)
{
	return(logger->time_option);
}
/* Gets the time that the ErrorLogger was created.
 *
 * Assumes 'logger' is not null. */
time_t ErrorLogger_get_startup_time(ErrorLogger* logger)
{
	return(logger->startup_time);
}
/* Gets the number of times the ErrorLogger has logged an error.
 * If more than SIZE_MAX errors have been logged, then the value
 * will be inaccurate.
 *
 * Assumes 'logger' is not null. */
size_t ErrorLogger_get_log_count(ErrorLogger* logger)
{
	return(logger->log_count);
}
/* Gets the timeout of the ErrorLogger. This is the time used
 * to wait for the mutex to be locked before failing.
 *
 * Assumes 'logger' is not null. */
int ErrorLogger_get_timeout(ErrorLogger* logger)
{
	return(logger->timeout);
}
/* Gets the maximum size a file can be (in bytes) before
 * it is truncated.
 *
 * Assumes 'logger' is not null. */
size_t ErrorLogger_get_max_file_size(ErrorLogger* logger)
{
	return(logger->max_file_size);
}
	/***********/

	/* Setters */
		/* Protected */
/* Sets the startup time for the ErrorLogger.
 * This is set upon object creation, but can be set to any desired value.
 *
 * Assumes 'logger' is not null. */
void ErrorLogger_set_startup_time(ErrorLogger* logger, time_t startup_time)
{
	*((time_t*)&logger->startup_time) = startup_time;
}
		/*************/

/* Sets the file path to the file that should be used for logging errors. */
void ErrorLogger_set_file_path(ErrorLogger* logger, const char* file_path)
{
	char* path_ptr;

	if(!logger || !file_path)return;

	pthread_mutex_lock(&logger->mutex);

	/* We do the switch-a-roo to ensure that it is safe.
	 * Freeing may take more than one cycle.  Even though we
	 * have the mutex locked, do this just to be double safe. */
	path_ptr = logger->file_path;
	logger->file_path = NULL;
	if(path_ptr)free(path_ptr);

	strcpy_alloc(&logger->file_path, file_path);
	pthread_mutex_unlock(&logger->mutex);
}
/* Sets the application name that will be used when logging errors. */
void ErrorLogger_set_app_name(ErrorLogger* logger, const char* app_name)
{
	char* name_ptr;

	if(!logger || !app_name)return;

	pthread_mutex_lock(&logger->mutex);
	name_ptr = (char*)logger->app_name;
	logger->file_path = NULL;
	if(name_ptr)free(name_ptr);

	strcpy_alloc((char**)(&logger->app_name), app_name);
	pthread_mutex_unlock(&logger->mutex);
}
/* Sets the logger's time option, this will determine what time data will be
 * written to the file upon error.
 *
 * Assumes 'logger' is not null. */
void ErrorLogger_set_time_option(ErrorLogger* logger, ErrorLoggerTimeOption option)
{
	*((ErrorLoggerTimeOption*)&logger->time_option) = option;
}
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
void ErrorLogger_set_timeout(ErrorLogger* logger, int timeout)
{
	logger->timeout = timeout;
}
/* Sets the maximum number of bytes a file can use before it is truncated.
 *
 * Assumes 'logger' is not null. */
void ErrorLogger_set_max_file_size(ErrorLogger* logger, size_t max_file_size)
{
	logger->max_file_size = max_file_size;
}

/* Sets the truncation algorithms to use the default callbacks based on size.
 * This decreases the size of the file by at least half by removing the
 * oldest logs first.  Max size is determined by DEFAULT_MAX_ELOGGER_SIZE.
 *
 * Assumes 'logger' is not null. */
void ErrorLogger_use_default_truncate_size_cb(ErrorLogger* logger)
{
	logger->truncate_cb = ErrorLogger_default_truncate_size;
	logger->truncate_ready_cb = ErrorLogger_default_truncate_size_ready;
}
/* Sets the truncation callback for the ErrorLogger.  This will be called
 * whenever the logger's 'truncate_ready_cb' returns true. If the function
 * is null, then the file will never be truncated.
 *
 * Assumes 'logger' is not null. */
void ErrorLogger_set_truncate_cb(ErrorLogger* logger, el_truncate_cb truncate_cb)
{
	logger->truncate_cb = truncate_cb;
}
/* Sets the truncation ready callback for the ErrorLogger.  This will be called
 * whenever the ErrorLogger prepares to append data to the file.  If the function
 * is null, then the file will never be truncated.
 *
 * Assumes 'logger' is not null. */
void ErrorLogger_set_truncate_ready_cb(ErrorLogger* logger, el_truncate_ready_cb truncate_ready_cb)
{
	logger->truncate_ready_cb = truncate_ready_cb;
}
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
		const char* app_name)
{
	if(!file_path)return(NULL);

	ErrorLogger* logger = malloc(sizeof(ErrorLogger));
	if(!logger)return(NULL);

	/* Initialize non-dynamic members. */
	*((ErrorLoggerTimeOption*)&logger->time_option) = time_option;
	*((time_t*)&logger->startup_time) = time(NULL);
	logger->log_count = 0;
	logger->file = NULL;
	logger->timeout = 500;
	logger->max_file_size = DEFAULT_MAX_ELOGGER_SIZE;
	pthread_mutex_init(&logger->mutex, NULL);

	ErrorLogger_use_default_truncate_size_cb(logger);

	/* Initialize dynamic members. */
	strcpy_alloc((char**)&logger->file_path, file_path);
	if(app_name)
		strcpy_alloc((char**)&logger->app_name, app_name);
	else
		logger->app_name = NULL;

	/* Check for errors. */
	if(!logger->file_path || !logger->app_name)
		delErrorLogger(&logger);

	return(logger);
}

/* Destroys an ErrorLogger object and sets the pointer to NULL. */
void delErrorLogger(ErrorLogger** logger)
{
	if(!logger || !*logger)
		return;

	pthread_mutex_destroy(&(*logger)->mutex);
	if((*logger)->app_name)
		free((char*)(*logger)->app_name);
	if((*logger)->file_path)
		free((char*)(*logger)->file_path);
	if((*logger)->file)
		fclose((*logger)->file);
	free(*logger);
	*logger = NULL;
}
/**************************/
