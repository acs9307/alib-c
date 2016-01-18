#include <stdio.h>
#include "DList.h"

void print_list(DList* list)
{
	printf("list_begin: %s\n", (DList_get_count(list))?
			(char*)ListItem_get_val(DList_get(list, 0)):"null");
	printf("list_end: %s\n", (DList_get_count(list))?
			(char*)ListItem_get_val(DList_get(list, DList_get_count(list) - 1)) : "null");
	printf("list_count: %d\n", (int)DList_get_count(list));
	printf("list contents:\n");
	{
		DListItem* dli = (DListItem*)DList_get(list, 0);
		for(; dli; dli = DListItem_get_next_item(dli))
			printf("\t%s\n", (char*)ListItem_get_val(((ListItem*)dli)));
	}

	printf("\n");
}

int main()
{
	DList* list = newDList();
	char* hello_world = "hello world!";
	DListItem* ht = newDListItem("Hey there!", NULL, NULL);
	DListItem* ha = newDListItem("hello again!", NULL, &ht);

	printf("---Push Back---\n");
		DList_push_back(list, newDListItem(hello_world, NULL, NULL));
		print_list(list);

	printf("---Push Forward---\n");
		DList_push_forward(list, newDListItem("wassup?", NULL, NULL));
		print_list(list);

	printf("---Insert---\n");
		DList_insert(list, 1, ht);
		print_list(list);

	printf("---Get---\n");
		ht = (DListItem*)DList_get(list, 1);
		printf("ht: %s\n", (char*)(ListItem_get_val((ListItem*)ht)));
		print_list(list);

	printf("---Pull Out---\n");
		ht = DList_pull_out(list, DListItem_index(ht));
		ha = DList_pull_out_count(list, 1, 2);
		printf("ht: %s, ht_count: %d\n", (char*)DListItem_get_value(ht),
				(int)DListItem_count(ht));
		printf("ha: %s, ha_count: %d\n", (char*)DListItem_get_value(ha),
				(int)DListItem_count(ha));
		print_list(list);

	printf("---Move---\n");
		DList_push_back(list, ht);
		DList_push_forward(list, ha);
		print_list(list);
		DList_move(list, 3, 0);
		DList_move_count(list, 2, 1, 3);
		print_list(list);

	printf("---Remove---\n");
		DList_remove(list, 1);
		DList_remove_item(list, ht);
		print_list(list);
		DList_remove_count(list, 0, 2);
		print_list(list);

	printf("\nApplication closing!\n");
	delDList(&list);
	return(0);
}
