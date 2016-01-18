#include "ArrayList.h"
#include "StringObject.h"
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
	StringObject* str;

	ArrayList_resize(list, 0);

	str = newStringObject();
	str->append(str, "hello world");
	ArrayList_add(list, StringObject_extract_c_string(str));
	str->append(str, "hello again!");
	ArrayList_add(list, StringObject_extract_c_string(str));

	print_array_data(list);

	ArrayList_resize(list, 1);
	printf("\n\nResized ArrayList to capacity of 1!\n");
	print_array_data(list);

	str->append(str, "hey there!");
	ArrayList_add(list, StringObject_extract_c_string(str));
	printf("\n\nAdded a string!\n");
	print_array_data(list);

	ArrayList_resize(list, 10);
	printf("\n\nResized ArrayList to capacity of 10!\n");
	print_array_data(list);

	delArrayList(&list);
	delStringObject(&str);
	return(0);
}
