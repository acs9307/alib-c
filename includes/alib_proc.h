#ifndef ALIB_PROC_IS_DEFINED
#define ALIB_PROC_IS_DEFINED

#include "alib_file.h"
#include "BinaryBuffer.h"
#include "String.h"

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
int get_proc_pids(const char** procNames, size_t procNameCount, int** pids);

#endif
