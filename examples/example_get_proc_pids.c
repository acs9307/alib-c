#include <stdio.h>

#include "alib_proc.h"

/* Simple example on how to use 'get_proc_pids()'. */

int main()
{
	int* pids, *pidIt;
	int pidCount = get_proc_pids((const char*[]){"/home/gurtha/workspace-cpp/alib-c/Debug/alib-c", "bash", NULL}, &pids);

	printf("%d pids found.\n", pidCount);
	printf("Found pids:\n");
	for(pidIt = pids; pidCount > 0; ++pidIt, --pidCount)
		printf("\t%d\n", *pidIt);

	if(pids)
		free(pids);

	printf("Application Closing!\n");
	return(0);
}

