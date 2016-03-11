#include "ArrayList.h"
#include "String.h"
#include <stdio.h>

void print_array_data(ArrayList* list)
{
	const char** strings = (const char**)ArrayList_get_array_ptr(list);
	size_t cap = ArrayList_get_capacity(list);

	printf("Array Data:\n");
	for(; cap > 0; --cap, ++strings)
		printf("\t%p: %s\n", *strings, *strings);

	printf("ArrayList Count: %d\n", (int)ArrayList_get_count(list));
	printf("ArrayList Capacity: %d\n", (int)ArrayList_get_capacity(list));
}

int main()
{
	ArrayList* list = newArrayList(free);
	String* str;

	ArrayList_resize(list, 0);

	/* Add. */
	str = newString();
	String_append(str, "hello world");
	ArrayList_add(list, String_extract_c_string(str));
	String_append(str, "hello again!");
	ArrayList_add(list, String_extract_c_string(str));

	print_array_data(list);

	/* Resize */
	ArrayList_resize(list, 1);
	printf("\n\nResized ArrayList to capacity of 1!\n");
	print_array_data(list);

	String_append(str, "hey there!");
	ArrayList_add(list, String_extract_c_string(str));
	printf("\n\nAdded a string!\n");
	print_array_data(list);

	ArrayList_resize(list, 10);
	printf("\n\nResized ArrayList to capacity of 10!\n");
	print_array_data(list);

	/* Contains */
		/* Add one to the string length so that we compare the null terminator as well.
		 * See below examples for why. */
	printf("contains 'hey there!': %d\n", ArrayList_contains(list, "hey there!", strlen("hey there!") + 1));
	printf("contains 'hey': %d\n", ArrayList_contains(list, "hey", strlen("hey")));
	printf("contains 'there!': %d\n", ArrayList_contains(list, "there!", strlen("there!") + 1));

	delArrayList(&list);
	delString(&str);
	return(0);
}

