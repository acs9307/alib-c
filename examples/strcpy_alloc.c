#include "alib_string.h"

int main()
{
	const char* str = "-D /home/gurtha/terminalAppDebug";
	char* buff[2];
	int i = 0;

	for(;str; ++i)
	{
		const char* end_ptr = find_next_whitespace(str);

		if(end_ptr)
			strncpy_alloc(&buff[i], str, end_ptr - str);
		else
			strcpy_alloc(&buff[i], str);

		str = find_next_non_whitespace(end_ptr);
	}

	for(i = 0; i < 2; ++i)
		printf("buff[%d]: %s\n", i, buff[i]);


	printf("Application closing!\n");
	return(0);
}
