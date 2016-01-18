#ifndef ERROR_LOGGER_PROTECTED_IS_DEFINED
#define ERROR_LOGGER_PROTECTED_IS_DEFINED

#include "ErrorLogger.h"

/* Setters */
	/* Protected */
/* Sets the startup time for the ErrorLogger.
 * This is set upon object creation, but can be set to any desired value.
 *
 * Assumes 'logger' is not null. */
void ErrorLogger_set_startup_time(ErrorLogger* logger, time_t startup_time);
	/*************/
/***********/

#endif
