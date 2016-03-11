#include "alib_proc.h"

/* Returns the PIDs of applications with a name matching 'procName'.
 *
 * Parameters:
 * 		procNames: Array of names to search for.  MUST BE A NULL TERMINATED
 * 			double char pointer.
 * 		procNameCount: The length of 'procNames'.
 * 		pids: A dynamically allocated array of pids.  This MUST BE FREED
 * 			by the caller.
 *
 * Returns:
 * 		>=0: The number of pids found.
 * 		 <0: alib_error */
int get_proc_pids(const char** procNames, size_t procNameCount, int** pids)
{
	if(!procNames || !pids)return(ALIB_BAD_ARG);

	const char** nameIter;
	size_t nameIterCount;
	BinaryBuffer* pidBuff = newBinaryBuffer();
	char fbuff[8 * 1024];
	int readCount;
	char** dirNames = NULL, **it;
	int itCount, err;
	int fileCount = get_all_file_names("/proc/", &dirNames, 1, 0);
	String* fnameParser = newString();

	/* Check for errors. */
	if(!fileCount)
	{
		err = ALIB_UNKNOWN_ERR;
		goto f_return;
	}
	else if(!pidBuff || !fnameParser)
	{
		err = ALIB_MEM_ERR;
		goto f_return;
	}

	/* pids should be set to NULL. */
	*pids = NULL;

	/* Search the directories and search for a matching
	 * command line string. */
	for(it = dirNames, itCount = fileCount; itCount > 0;
			--itCount, ++it)
	{
		if(!*it)continue;

		int dirNum = atoi(*it);
		if(dirNum > 0)
		{
			FILE* cmdline = NULL;

			String_set(fnameParser, "/proc/");
			String_append_int(fnameParser, dirNum);
			String_append(fnameParser, "/cmdline");

			cmdline = fopen(String_get_c_string(fnameParser), "r");
			if(!cmdline)goto if_break;

			readCount = fread(fbuff, 1, sizeof(fbuff), cmdline);
			if(readCount > 0)
			{
				for(nameIter = procNames, nameIterCount = 0; nameIterCount < procNameCount;
						++nameIter, ++nameIterCount)
				{
					/* A null terminator may be at the end of the buffer.
					 * If so, then we decrement the count. */
					if(fbuff[readCount - 1] == 0)
						--readCount;

					if(str_match(fbuff, readCount, *nameIter,
							strlen(*nameIter)) == 0)
					{
						BinaryBuffer_append(pidBuff, &dirNum, sizeof(dirNum));
					}
				}
			}

		if_break:
			if(cmdline)
				fclose(cmdline);
		}
	}

	err = BinaryBuffer_get_length(pidBuff) / sizeof(int);
	*pids = BinaryBuffer_extract_buffer(pidBuff);

f_return:
	delBinaryBuffer(&pidBuff);
	delString(&fnameParser);

	/* Free all the char pointers that we allocated. */
	for(it = dirNames, itCount = fileCount;
			itCount > 0; --itCount, ++it)
	{
		if(*it)free(*it);
	}

	if(dirNames)
		free(dirNames);

	return(err);
}
