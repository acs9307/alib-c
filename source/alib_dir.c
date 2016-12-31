#include "includes/alib_dir.h"

/* Points to a dynamically allocated string that represents the file
 * location of the currently running executable.
 *
 * To retrieve fill the string, call 'get_app_file_dir_from_argv(const char**)'.
 * To free the string, call 'free_app_file_dir()'. */
const char* APP_FILE_DIR = NULL;

/* Reads the application directory path from the application's first argument
 * and sets the APP_FILE_DIR to represent that string.
 *
 * The string is copied into memory and will be freed only if it is re-written
 * or 'free_app_file_dir()' is called.
 *
 * Parameters:
 * 		argv: This MUST be pointing to the first argument, otherwise the value
 * 			of APP_FILE_DIR will be incorrect. */
void get_app_file_dir_from_argv(char** argv)
{
	if(argv && *argv)
	{
		char* dir_end = find_last_char(*argv, '/');
		if(!dir_end)
			dir_end = find_last_char(*argv, '\\');

		if(APP_FILE_DIR)
			free((char*)APP_FILE_DIR);

		*((char**)&APP_FILE_DIR) = make_substr(*argv, ++dir_end);
	}
}

/* Frees any memory that may have been allocated for the use of
 * APP_FILE_DIR and sets the pointer to NULL. */
void free_app_file_dir()
{
	if(APP_FILE_DIR)
	{
		free((char*)APP_FILE_DIR);
		*((char**)&APP_FILE_DIR) = NULL;
	}
}
