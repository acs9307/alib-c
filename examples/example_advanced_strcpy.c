#include <stdio.h>

#include "alib_string.h"

#define STRING1 "hello world"
#define STRING2 "hello there"


char str1[50] = STRING1;
char str2[50] = STRING2;

void print_string1()
{
	printf("str1: %s\n", str1);
	strcpy(str1, STRING1);
}
void print_strings()
{
	print_string1();
	printf("str2: %s\n", str2);
	printf("\n\n");

	strcpy(str2, STRING2);
}

void test1()
{
	char* str1_ptr, *str2_ptr;
	size_t len = strlen(str1);

	printf("Using strcpy(str1_ptr, str2_ptr)\n");
	for(str1_ptr = str1,str2_ptr = str1 + len;
			1; ++str1_ptr, --str2_ptr)
	{
		strcpy(str1_ptr, str2_ptr);
		print_string1();

		printf("\tstr1_ptr: %c, %d\n", (*str1_ptr)?*str1_ptr:'~', *str1_ptr);
		printf("\tstr2_ptr: %c, %d\n", (*str2_ptr)?*str2_ptr:'~', *str2_ptr);

		if(str2_ptr == str1)
			break;
	}
	printf("\n");

	printf("Using strcpy_back(str1_ptr, str2_ptr)\n");
	for(str1_ptr = str1,str2_ptr = str1 + len;
			1; ++str1_ptr, --str2_ptr)
	{
		strcpy_back(str1_ptr, str2_ptr);
		print_string1();

		printf("\tstr1_ptr: %c, %d\n", (*str1_ptr)?*str1_ptr:'~', *str1_ptr);
		printf("\tstr2_ptr: %c, %d\n", (*str2_ptr)?*str2_ptr:'~', *str2_ptr);

		if(str2_ptr == str1)
			break;
	}
	printf("\n");
}

void test2()
{
	char* str1_ptr, *str2_ptr;
	size_t len = strlen(str1);

	printf("\n\nTEST2\n\n");

	printf("Using strcpy_safe(str1_ptr, str2_ptr)\n");
	for(str1_ptr = str1,str2_ptr = str1 + len;
			1; ++str1_ptr, --str2_ptr)
	{

		strcpy_safe(str1_ptr, str2_ptr);
		print_string1();

		printf("\tstr1_ptr: %c, %d\n", (*str1_ptr)?*str1_ptr:'~', *str1_ptr);
		printf("\tstr2_ptr: %c, %d\n", (*str2_ptr)?*str2_ptr:'~', *str2_ptr);

		if(str2_ptr == str1)
			break;
	}

	printf("\n");
}

int main()
{
	test1();
	test2();

	printf("Application exiting!\n");
	return(0);
}
