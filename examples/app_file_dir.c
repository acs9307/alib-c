#include "alib_dir.h"

int main(int argc, char** argv)
{
	printf("Getting app directory.\n");
	printf("argv: %s\n", *argv);
	get_app_file_dir_from_argv(argv);
	printf("APP_FILE_DIR before free: %s\n", (APP_FILE_DIR)?APP_FILE_DIR:"null");

	free_app_file_dir();
	printf("APP_FILE_DIR after free: %s\n", (APP_FILE_DIR)?APP_FILE_DIR:"null");

	printf("Application returning!\n");
	return(0);
}
