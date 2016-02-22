#include "String.h"

void print_strings(String* str1, String* str2)
{
	if(str1)
		printf("str1: %s\n", String_get_c_string(str1));
	if(str2)
		printf("str2: %s\n", String_get_c_string(str2));

	printf("\n\n");
}

int main()
{
	String* s = newString();
	String* s_ex = newString_ex("hello extended world", 0);

	printf("---Constructors---\n");
	print_strings(s, s_ex);

	printf("---Set---\n");
	String_set(s, "hello world");
	String_set(s_ex, "hello again");
	print_strings(s, s_ex);

	printf("---Append---\n");
	String_append(s, ", how are you?");
	String_append_count(s_ex, ", now go away!", strlen(", now go away!"));
	print_strings(s, s_ex);

	printf("---Insert---\n");
	String_insert(s, " nice", 5);
	String_insert(s_ex, "Well, ", 0);
	String_insert_count(s_ex, " ... Are you still there!?! :S", String_get_length(s_ex),
			strlen(" ... Are you still there!?! :S"));
	print_strings(s, s_ex);

	printf("---Copy---\n");
	String_copy_string(s_ex, s);
	print_strings(s, s_ex);

	printf("---To Lower/Upper---\n");
	String_to_lower(s);
	String_to_upper(s_ex);
	print_strings(s, s_ex);

	printf("---Shrink---\n");
	printf("s_capacity before: %d\n", (int)String_get_capacity(s));
	String_shrink_to_fit(s);
	printf("s_capacity after: %d\n", (int)String_get_capacity(s));
	print_strings(s, s_ex);

	printf("---Replace---\n");
	String_replace(s, 11, -1, "WORLD");
	String_replace_count(s_ex, 0, 5, "hello", 5);
	String_replace_count(s_ex, String_get_length(s_ex) - 4, 3, "you!", 4);
	String_replace_substrings(s, "nice", "icy", 0);
	String_replace_substrings(s, "HOW ", "", 0);
	print_strings(s, s_ex);

	printf("---Count Substrings---\n");
	printf("Counting 'l's: %d\n", (int)String_count_substrings(s, "l"));
	printf("Counting 'WOR's: %d\n", (int)String_count_substrings(s_ex, "WOR"));
	print_strings(s, s_ex);

	printf("---Substring Pointers---\n");
	{
		char* const* substring_array = String_get_substring_ptrs(s, "WORLD");
		char* const* array_it = substring_array;
		size_t array_count;

		printf("Substrings of 's'.\n");
		for(array_count = 0; *array_it; ++array_it, ++array_count)
			printf("\t%s\n", *array_it);
		printf("Array count: %d\n", (int)array_count);
		free((char**)substring_array);

		array_it = substring_array = String_get_substring_ptrs(s_ex, "l");
		printf("Substrings of 's_ex'.\n");
		for(array_count = 0; *array_it; ++array_it, ++array_count)
			printf("\t%s\n", *array_it);
		printf("Array count: %d\n", (int)array_count);
		free((char**)substring_array);
	}
	print_strings(s, s_ex);

	printf("---Remove---\n");
	String_remove(s, 6, 10);
	String_remove_substring(s_ex, " NICE");
	print_strings(s, s_ex);

	printf("---Extract---\n");
	printf("s->len before: %d\n", (int)String_get_length(s));
	printf("s_ex->len before: %d\n", (int)String_get_length(s_ex));
	{
		char* s_raw = String_extract_c_string(s);
		char* s_ex_raw = String_extract_c_string(s_ex);

		printf("s_raw: %s\n", s_raw);
		printf("s_ex_raw: %s\n", s_ex_raw);
		free(s_raw);
		free(s_ex_raw);
	}
	printf("s->len after: %d\n", (int)String_get_length(s));
	printf("s_ex->len after: %d\n", (int)String_get_length(s_ex));
	print_strings(s, s_ex);

	delString(&s);
	delString(&s_ex);

	printf("Application returning!\n");
	return(0);
}
