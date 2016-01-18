#include <stdio.h>
#include "DListItem.h"

void print_list(DListItem* li, char* ptr_name)
{
	printf("Printing pointer '%s'.\n", ptr_name);
	printf("list_ptr: %s\n", (char*)DListItem_get_value(li));
	printf("list_begin: %s\n", (char*)DListItem_get_value(
			DListItem_get_first_item(li)));
	printf("list_end: %s\n", (char*)DListItem_get_value(
			DListItem_get_last_item(li)));
	printf("list_count: %d\n", (int)DListItem_count(li));
	printf("list contents:\n");
		li = DListItem_get_first_item(li);
		for(; li; li = DListItem_get_next_item(li))
			printf("\t%s\n", (char*)((ListItemVal*)li)->value);

	printf("\n");
}

int main()
{
	char* hello_world = "hello world!";
	DListItem* hw = newDListItem(hello_world, NULL, NULL);
	DListItem* wu = newDListItem("wassup?", NULL, &hw);
	DListItem* ht = newDListItem("Hey there!", NULL, NULL);
	DListItem* ha = newDListItem("hello again!", NULL, NULL);

	printf("---newDListItem---\n");
	print_list(hw, "hw");

	printf("---Insertion---\n");
	printf("insert ht: %s\n", (char*)DListItem_get_value(
			DListItem_insert_after(hw, ht)));
	printf("insert ha: %s\n", (char*)DListItem_get_value(
			DListItem_insert_before(hw, ha)));
	print_list(ha, "ha");

	printf("---PullOut---\n");
	hw = DListItem_get(hw, 2);
	wu = DListItem_pull_out(&hw);
	ha = DListItem_pull_out_count(&hw, 2);
	printf("wu - count: %d, val: %s\n", (int)DListItem_count(wu),
			(char*)DListItem_get_value(wu));
	printf("ha - count: %d, val: %s\n", (int)DListItem_count(ha),
				(char*)DListItem_get_value(ha));
	print_list(hw, "hw");

	printf("---Push Back/Forward---\n");
	hw = DListItem_push_back(hw, wu);
	hw = DListItem_push_forward(hw, ha);
	print_list(hw, "hw");

	printf("---Get---\n");
	hw = DListItem_get_by_value(hw, hello_world);
	printf("hw value = %s, hw index = %d\n", (char*)DListItem_get_value(hw),
			DListItem_index(hw));
	wu = DListItem_get_by_relative_index(hw, 1);
	printf("wu value = %s, wu index = %d\n", (char*)DListItem_get_value(wu),
			DListItem_index(wu));
	ha = DListItem_get_by_relative_index(hw, -2);
	printf("ha value = %s, ha index = %d\n", (char*)DListItem_get_value(ha),
			DListItem_index(ha));

	printf("\nApplication closing!\n");
	delDListItem_all(&hw);
	return(0);
}
