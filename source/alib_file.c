#include "alib_file.h"

/*------------FILE SEARCHING------------*/

/*******CALLBACKS*******/
/* Callback for 'qsort()'.  Sorts two file names by comparing their
 * integer number value.  If the file names cannot be parsed into an integer, then
 * results are undefined. */
int cb_file_sort_by_number(const void* a, const void* b)
{
	int64_t a_val = atoll(*((const char**)a));
	int64_t b_val = atoll(*((const char**)b));

	if(a_val < b_val)return(-1);
	if(a_val > b_val)return(1);
	return(0);
}
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
char mkdir_ex(const char* dir, mode_t mode, char recurs)
{
	if(!recurs)
		return(mkdir(dir, mode));
	else
	{
		StringObject* dir_str = newStringObject();
		char rval;
		char* ptr;

		/* Check for error in allocating memory. */
		if(!dir_str)return(-1);
		dir_str->append(dir_str, dir);

		/* Ensure that the last character is a null terminator. */
		ptr = dir_str->str + dir_str->length - 1;
		while(*ptr == '/' || *ptr == '\\')
		{
			*ptr = '\0';
			--ptr;
		}

		/* Iterate through the string and create each directory. */
		for(ptr = dir_str->str + 1;*ptr != '\0'; ++ptr)
		{
			if(*ptr == '/' || *ptr == '\\')
			{
				*ptr = '\0';
				mkdir(dir_str->str, mode);
				*ptr = '/';
			}
		}
		rval = mkdir(dir_str->str, mode);

		delStringObject(&dir_str);
		return(rval);
	}
}

/* Creates the directory if it does not exist.
 *
 * Returns:
 * 		1: Success
 * 		0: Error
 */
char create_directory_if_not_exists(const char* dir)
{
	/* Make sure the directory is available. */
	if(!dir)
		return(0);
	else
	{
		struct stat st = {0};
		if(stat(dir, &st) == -1)
		{
			if(mkdir_ex(dir, 0777, 1))
				return(0);
		}
		return(1);
	}
}

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
unsigned long long highest_value_file(const char* directory)
{
	unsigned long long  highest_value = 0;
	DIR* d;
	struct dirent* dir;

	d = opendir(directory);
	if(d)
	{
		/* Check all the files. */
		while((dir = readdir(d)))
		{
			/* Only read normal files. */
			if(dir->d_type == DT_REG && *dir->d_name != '.')
			{
				unsigned long long  fn = strtoll(dir->d_name, NULL, 10);
				if(fn > highest_value)
					highest_value = fn;
			}
		}

		closedir(d);
	}
	return(highest_value);
}
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
unsigned long long lowest_value_file(const char* directory)
{
	unsigned long long lowest_value = 0;
	DIR* d;
	struct dirent* dir;

	d = opendir(directory);
	if(d)
	{
		/* Read all the files. */
		while((dir = readdir(d)))
		{
			/* Only read regular files, we will not worry about directory files. */
			if(dir->d_type == DT_REG && *dir->d_name != '.')
			{
				unsigned long long fn = strtoll(dir->d_name, NULL, 10);
				if(fn < lowest_value || lowest_value == 0)
					lowest_value = fn;
			}
		}

		closedir(d);
	}
	return(lowest_value);
}

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
int get_file_count(const char* directory, char include_directories, char include_hidden)
{
	DIR* d;
	struct dirent* dir;
	int file_count = 0;	//Number of files found in the directory.

	/* Check for null argument. */
	if(!directory)
		return(-1);

	/* Open the directory. */
	d = opendir(directory);
	if(d)
	{
		/* Iterate through all the files in the directory. Increment
		 * file_count for each file found. */
		while((dir = readdir(d)))
		{
			if(dir->d_type == DT_REG)
			{
				/* Check to see if it is a hidden file. */
				if(!include_hidden && *dir->d_name == '.')
					continue;
				++file_count;
			}
			else if(dir->d_type == DT_DIR && include_directories)
				++file_count;
		}

		/* Close the directory. */
		closedir(d);
	}
	else
		file_count = ALIB_FILE_OPEN_ERR;

	return(file_count);
}

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
		char include_hidden)
{
	DIR* d;
	struct dirent* dir;
	int file_count = get_file_count(directory, include_directories, include_hidden);

	/* Used for iterating through the 'r_file_names' array. */
	char** file_name_it;
	int file_it_counter;

	/* Ensure the arguments are not null and that there are in fact
	 * files in the given directory. */
	if(file_count < 0 || !r_file_names)return(ALIB_BAD_ARG);
	else if(!file_count)
	{
		*r_file_names = NULL;
		return(file_count);
	}

	/* Allocate the memory to store the file names. */
	*r_file_names = calloc(file_count, sizeof(char*));
	if(!*r_file_names)
		return(ALIB_MEM_ERR);

	/* Open the directory for searching. */
	d = opendir(directory);
	if(d)
	{
		/* Iterate through all the files in the directory. */
		for(file_it_counter = file_count, file_name_it = *r_file_names;
				file_it_counter > 0 && (dir = readdir(d));)
		{
			/* Check to see if it is a desired file.  */
			if(dir->d_type == DT_REG || (include_directories && dir->d_type == DT_DIR))
			{
				/* Check to see if it is a hidden file.  */
				if(*dir->d_name == '.' && !include_hidden)
					continue;

				/* Allocate memory*/
				*file_name_it = malloc(strlen(dir->d_name) + 1); //Add 1 for null-terminator
				if(!*file_name_it)
				{
					closedir(d);
					file_count = ALIB_MEM_ERR;
					goto f_error;
				}

				/* Copy the memory over. */
				strcpy(*file_name_it, dir->d_name);
				++file_it_counter;
				++file_name_it;
			}
		}

		closedir(d);
	}
	else
	{
		file_count = ALIB_FILE_OPEN_ERR;
		goto f_error;
	}

	goto f_return;
f_error:
	/* Free any memory allocated. */
	for(file_name_it = *r_file_names; *file_name_it != NULL; ++file_name_it)
		free(*file_name_it);
	free(*r_file_names);
	*r_file_names = NULL;

f_return:
	return(file_count);
}

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
FILE* fopen_i(const char* dir, int64_t name, const char* open_type)
{
	char* f_name;
	FILE* r_file;

	if(!dir || !open_type)
		return(NULL);

	/* Allocate the directory length plus the max int length plus one for null terminator plus one
	 * for '/' character. */
	f_name = malloc(strlen(dir) + LLONG_MAX_CHAR_LEN + 2);
	if(!f_name)return(NULL);
	sprintf(f_name, "%s/%lld", dir, (long long int)name);

	/* Open the file. */
	r_file = fopen(f_name, open_type);

	return(r_file);
}

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
FILE* fopen_ui(const char* dir, uint64_t name, const char* open_type)
{
	char* f_name;
	FILE* r_file;

	if(!dir || !open_type)
		return(NULL);

	/* Allocate the directory length plus the max int length plus one for null terminator plus one
	 * for '/' character. */
	f_name = malloc(strlen(dir) + LLONG_MAX_CHAR_LEN + 2);
	if(!f_name)return(NULL);
	sprintf(f_name, "%s/%llu", dir, (unsigned long long)name);

	/* Open the file. */
	r_file = fopen(f_name, open_type);

	return(r_file);
}

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
size_t file_size(FILE* file)
{
	size_t cur_pos;
	size_t size;

	if(!file)return(0);
	cur_pos = ftell(file);

	fseek(file, 0, SEEK_END);
	size = ftell(file);
	fseek(file, cur_pos, SEEK_SET);
	return(size);
}

/*----------------------------------*/

/*------------FILE MODIFICATION------------*/

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
alib_error file_remove_data_at(const char* file_name, size_t pos, size_t bytes, file_remove_option option)
{
	FILE* file;
	char* buff;
	size_t new_size;
	size_t my_pos;
	alib_error err = ALIB_OK;

	/* Check for bad argument. */
	if(!file_name)return(ALIB_BAD_ARG);

	/* Open the file to modify. */
	file = fopen(file_name, "r");
	if(!file)return(ALIB_FILE_OPEN_ERR);

	/* Get the file size and calculate the new file size. */
	new_size = file_size(file);
	if(!new_size)return(ALIB_OK);
	if(!bytes)
	{
		if(option == FRO_BEGIN || option == FRO_CUR_BEFORE)
			bytes = pos;
		else if(option == FRO_CUR_AFTER || option == FRO_END)
			bytes = new_size - pos;
	}
	if(new_size < bytes)return(ALIB_BAD_ARG);
	new_size -= bytes;

	/* Allocate the buffer to store the memory in temporarily. */
	buff = malloc(new_size);
	if(!buff)return(ALIB_MEM_ERR);

	/* Copy the file data into the buffer. */
	switch(option)
	{
	case FRO_BEGIN:
		/* Copy the last section of data. */
		fseek(file, bytes, SEEK_SET);
		if(fread(buff, 1, new_size, file) != new_size)
		{
			err = ALIB_FILE_READ_ERR;
			goto f_return;
		}
		break;
	case FRO_CUR_BEFORE:
		/* Copy all but the number of 'bytes' before the current position. */
		fseek(file, 0, SEEK_SET);
		my_pos = pos - bytes;
		if(fread(buff, 1, my_pos, file) != my_pos)
		{
			err = ALIB_FILE_READ_ERR;
			goto f_return;
		}
		fseek(file, bytes, SEEK_CUR);
		if(fread(buff + my_pos, 1, new_size - my_pos, file) != new_size - my_pos)
		{
			err = ALIB_FILE_READ_ERR;
			goto f_return;
		}
		break;
	case FRO_CUR_AFTER:
		/* Copy all but the number of 'bytes' after the current position. */
		fseek(file, 0, SEEK_SET);
		if(fread(buff, 1, pos, file) != pos)
		{
			err = ALIB_FILE_READ_ERR;
			goto f_return;
		}
		fseek(file, bytes, SEEK_CUR);
		if(fread(buff + pos, 1, new_size - pos, file) != new_size - pos)
		{
			err = ALIB_FILE_READ_ERR;
			goto f_return;
		}
		break;
	case FRO_END:
		/* Copy all the data from the beginning of the file. */
		fseek(file, 0, SEEK_SET);
		if(fread(buff, 1, new_size, file) != new_size)
		{
			err = ALIB_FILE_READ_ERR;
			goto f_return;
		}
		break;
	default:
		err = ALIB_BAD_ARG;
		goto f_return;
	}

	/* Close the file and reopen it with truncating
	 * write permissions. */
	fclose(file);
	file = fopen(file_name, "w+");
	if(!file)
	{
		err = ALIB_FILE_OPEN_ERR;
		goto f_return;
	}
	/* Write the data. */
	if(fwrite(buff, 1, new_size, file) != new_size)
	{
		err = ALIB_FILE_ERR;
		goto f_return;
	}

f_return:
	if(buff)
		free(buff);
	if(file)
		fclose(file);

	return(err);
}

/*-----------------------------------------*/
