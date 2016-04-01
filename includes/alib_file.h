#ifndef ALIB_FILE_IS_INCLUDED
#define ALIB_FILE_IS_INCLUDED

#include <inttypes.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <inttypes.h>
#include <stdio.h>

#include "StringObject.h"
#include "alib_error.h"

/* Part of alib.  Contains helper functions for dealing with files. */

/*------------FILE SEARCHING------------*/

/*******CALLBACKS*******/
/* Callback for 'qsort()'.  Sorts two file names by comparing their
 * integer number value.  If the file names cannot be parsed into an integer, then
 * results are undefined. */
int cb_file_sort_by_number(const void* a, const void* b);
/***********************/

/* Works the same way as 'mkdir()' except that it will allow
 * for recursive creation of parent directories.  If, for some reason,
 * the creation of the lowest child directory fails, then the function will
 * return the error code returned by 'mkdir()' (usually -1) and all
 * directories created will remain.
 *
 * Parameters:
 * 		dir: The full path of directories to make.
 * 		mode: The flags used for creating the directories.
 * 		recurs: If !0, all parent directories will be created.
 * 			If 0, function simply calls 'mkdir()'.
 *
 * Returns:
 * 		Returns the return code from 'mkdir()' which should
 * 		set errno.  The return value should be -1 for error
 * 		and 0 for success. */
char mkdir_ex(const char* dir, mode_t mode, char recurs);

/* Creates the directory if it does not exist.
 *
 * Returns:
 * 		1: Success
 * 		0: Error
 */
char create_directory_if_not_exists(const char* dir);

/* Gets the highest file number in the given directory.
 * Calculates the number using strtoll(), so only the first
 * part of the file name containing a number will be assumed as
 * the file's number.
 *
 * Does not calculate directory file numbers.
 *
 * Parameters:
 * 		directory: The full path to the directory to check.
 *
 * Returns:
 * 		The highest file number found. If zero is returned, then it is
 * 		assumed no numbered files exist.
 */
unsigned long long highest_value_file(const char* directory);
/* Gets the lowest file number in the given directory.
 * Calculates the number using strtoll(), so only the first
 * part of the file name containing a number will be assumed as
 * the file's number.
 *
 * Does not calculate directory file numbers.
 *
 * Parameters:
 * 		directory: The full path to the directory to check.
 *
 * Returns:
 * 		The lowest file number found. If zero is returned, then it is
 * 		assumed no numbered files exist.
 */
unsigned long long lowest_value_file(const char* directory);

/* Returns the number of files in a given directory.
 *
 * Parameters:
 * 		directory: The directory path to search in.
 * 		include_directories: If !0, child directories will be added to the
 * 			file count.
 * 		include_hidden: If !0, hidden files will be added to the file count.
 *
 * Returns:
 * 		>=0: The number of files found.
 * 		<0 : Error occurred. */
int get_file_count(const char* directory, char include_directories, char include_hidden);

/* Gets all the names of the files within the given directory and
 * places them into the given C-string array (pointer to an array of char*).
 * 'r_file_names' will be set to newly allocated memory and should be freed by the caller.
 * If 'r_file_names' was previously pointing to dynamically allocated memory, it should
 * be freed before calling this function.
 *
 * Parameters:
 * 		dir: The full path to the directory to search in.
 * 		r_file_names: The names of the files found.
 * 		include_directories: If !0, then sub directories will be included
 * 				in the list of return file names.
 * 		include_hidden: If !0, then hidden files will be included in the list
 * 				of return file names.
 *
 * Returns:
 * 		 <0: Error
 * 		>=0: Number of files found.
 */
int get_all_file_names(const char* directory, char*** r_file_names, char include_directories,
		char include_hidden);

/*--------------------------------------*/

/*------------FILE OPENING------------*/

/* Opens a file with the name based on the given integer and directory
 * path.  The method for opening depends on 'open_type'.
 *
 * Note:
 * 		This uses a signed integer not an unsigned integer.  If no negative sign
 * 			is required, it is suggested to use fopen_ui().
 *
 * Parameters:
 * 		dir: Full directory path that the file should be in.
 * 		name: The integer name of the file.
 * 		open_type: The method used to open the file, ("w" for write, "a" for append, same as fopen).
 *
 * Returns:
 * 		NULL: Error, could not open file.  Either invalid argument, could not allocate memory, or
 * 				could not open file.
 * 		FILE*: The pointer to the successfully opened file.
 */
FILE* fopen_i(const char* dir, int64_t name, const char* open_type);

/* Opens a file with the name based on the given integer and directory
 * path.  The method for opening depends on 'open_type'.
 *
 * Note:
 * 		This uses an unsigned integer not a signed integer.  If a negative
 * 			sign is required, then you should use fopen_i().
 *
 * Parameters:
 * 		dir: Full directory path that the file should be in.
 * 		name: The integer name of the file.
 * 		open_type: The method used to open the file, ("w" for write, "a" for append, same as fopen).
 *
 * Returns:
 * 		NULL: Error, could not open file.  Either invalid argument, could not allocate memory, or
 * 				could not open file.
 * 		FILE*: The pointer to the successfully opened file.
 */
FILE* fopen_ui(const char* dir, uint64_t name, const char* open_type);

/*------------------------------------*/

/*------------FILE STATS------------*/
/* Safely returns the size of the file. Only supports up to 'size_t'
 * sized files.
 *
 * Parameters:
 * 		file: The OPENED file to find the size of.
 *
 * Returns:
 * 		0: File could not be opened.
 * 		>0: The size of the file. */
size_t file_size(FILE* file);
/*----------------------------------*/

/*------------FILE MODIFICATION------------*/

typedef enum file_remove_option
{
	FRO_BEGIN = 1,
	FRO_CUR_BEFORE = 2,
	FRO_CUR_AFTER = 4,
	FRO_END = 8
}file_remove_option;

/* Removes a section of data from a file based on the given position.
 * If other operations are being conducted on the given file concurrently,
 * behavior is undefined.
 *
 * Parameters:
 * 		file_name: (REQUIRED) The name of the file to modify.
 * 		pos: (OPTIONAL) The position related to the section to remove.
 * 			If the remove option is set to FRO_BEGIN or FRO_END, this is not important.
 * 		bytes: (OPTIONAL) The number of bytes to remove from the file.
 * 			If 0, then all the data from one direction (depending on 'option')
 * 			will be removed.
 * 		option: (REQUIRED) The direction to remove data.
 */
alib_error file_remove_data_at(const char* file_name, size_t pos,
		size_t bytes, file_remove_option option);
/*-----------------------------------------*/

/*------------File Path Parsing------------*/
/* Returns the name of a file from the given path.
 * Supports both '/' and '\' directory delimiters.
 *
 * If the path is a directory path and does not include a trailing
 * directory delimiter, then the name of the directory will be returned.
 *
 * If the final character in the path is a directory delimiter, then the
 * return string will be an empty string.
 *
 * Whitespaces are treated as part of the string.
 *
 * Parameters:
 * 		path: The path to evaluate.
 * 		count: The number of characters to evaluate.
 * 			If 0, 'strlen()' will be used on 'path' to set 'count'.
 *
 * Returns:
 * 		Returns the name of the file from the given path.  If no directory
 * 		delimiters are contained within 'path', 'path' will be returned.
 *
 * 		The returned value is a substring of 'path', therefore no memory is
 * 		copied or allocated. */
const char* file_name_from_path_count(const char* path, size_t count);
/* Same as 'file_name_from_path_count(path, strlen(path))'. */
const char* file_name_from_path(const char* path);
/*-----------------------------------------*/

#endif
