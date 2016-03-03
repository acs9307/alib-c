#include "ComDataCheck.h"

#include <stdio.h>

int main()
{
	ComDataCheck* cdc = newComDataCheck(1);

	ComDataCheck_clear(cdc);
	ComDataCheck_clear(cdc);
	ComDataCheck_clear(cdc);

	ComDataCheck_append(cdc, "hello world", strlen("hello world"));
	ComDataCheck_clear(cdc);
	ComDataCheck_clear(cdc);
	ComDataCheck_clear(cdc);

	ComDataCheck_append(cdc, "hello world", strlen("hello world"));
	ComDataCheck_clear(cdc);
	ComDataCheck_clear(cdc);
	ComDataCheck_clear(cdc);


	printf("Application closing.\n");
	return(0);
}
