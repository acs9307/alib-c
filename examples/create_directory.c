#include "alib_file.h"

int main()
{
	printf("create_dir: %d\n",
			create_directory_if_not_exists("/home/gurtha/tmp/1/2/3/4/1/"));

	printf("Finished!\n");
	return(0);
}
