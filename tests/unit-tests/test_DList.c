#include <stdio.h>
#include <assert.h>
#include "DList.h"
#include "DList_private.h"
#include "ListItem.h"
#include "ListItem_private.h"
#include "DListItem.h"
#include "DListItem_private.h"

void test_newDList()
{
    // Declare variables
    DList* list;
    DListItem* item1;
    DListItem* item2;
    DListItem* item3;

    // Create a new DList
    list = newDList();

    // Check if the list is empty
    assert(DList_get_count(list) == 0);
    assert(DList_get_begin(list) == NULL);
    assert(DList_get_end(list) == NULL);

    // Push back an item
    int value1 = 10;
    item1 = newDListItem(&value1, NULL, NULL);
    DList_push_back(list, item1);
    assert(DList_get_count(list) == 1);
    assert(DList_get_begin(list) == item1);
    assert(DList_get_end(list) == item1);

    // Push front an item
    int value2 = 20;
    item2 = newDListItem(&value2, NULL, NULL);
    DList_push_forward(list, item2);
    assert(DList_get_count(list) == 2);
    assert(DList_get_begin(list) == item2);
    assert(DList_get_end(list) == item1);

    // Insert an item at index 1
    int value3 = 30;
    item3 = newDListItem(&value3, NULL, NULL);
    DList_insert(list, 1, item3);
    assert(DList_get_count(list) == 3);
    assert(DList_get_begin(list) == item2);
    assert(DList_get_end(list) == item1);

    // Remove an item at index 1
    DList_remove(list, 1);
    assert(DList_get_count(list) == 2);
    assert(DList_get_begin(list) == item2);
    assert(DList_get_end(list) == item1);

    // Clean up the DList
    delDList(&list);

    printf("All tests passed!\n");
}


void test_DList_push_back_null_list()
{
    DListItem *item = newDListItem(NULL, NULL, NULL);
    alib_error err = DList_push_back(NULL, item);
    assert(err == ALIB_BAD_ARG);
    delDListItem(&item);
}

void test_DList_push_back_null_item()
{
    DList *list = newDList();
    alib_error err = DList_push_back(list, NULL);
    assert(err == ALIB_BAD_ARG);
    delDList(&list);
}

void test_DList_push_back_item_with_parent()
{
    DList *list = newDList();
    DListItem *item = newDListItem(NULL, NULL, NULL);
    item->base.parent = (ListItem *)list;
    alib_error err = DList_push_back(list, item);
    assert(err == ALIB_BAD_ARG);
    delDList(&list);
    delDListItem(&item);
}

void test_DList_push_forward_null_list()
{
    DListItem *item = newDListItem(NULL, NULL, NULL);
    alib_error err = DList_push_forward(NULL, item);
    assert(err == ALIB_BAD_ARG);
    delDListItem(&item);
}

void test_DList_push_forward_null_item()
{
    DList *list = newDList();
    alib_error err = DList_push_forward(list, NULL);
    assert(err == ALIB_BAD_ARG);
    delDList(&list);
}

void test_DList_push_forward_item_with_parent()
{
    DList *list = newDList();
    DListItem *item = newDListItem(NULL, NULL, NULL);
    item->base.parent = (ListItem *)list;
    alib_error err = DList_push_forward(list, item);
    assert(err == ALIB_BAD_ARG);
    delDList(&list);
    delDListItem(&item);
}

void test_DList_insert_null_list()
{
    DListItem *item = newDListItem(NULL, NULL, NULL);
    alib_error err = DList_insert(NULL, 0, item);
    assert(err == ALIB_BAD_ARG);
    delDListItem(&item);
}

void test_DList_insert_null_item()
{
    DList *list = newDList();
    alib_error err = DList_insert(list, 0, NULL);
    assert(err == ALIB_BAD_ARG);
    delDList(&list);
}

void test_DList_insert_item_with_parent()
{
    DList *list = newDList();
    DListItem *item = newDListItem(NULL, NULL, NULL);
    item->base.parent = (ListItem *)list;
    alib_error err = DList_insert(list, 0, item);
    assert(err == ALIB_BAD_ARG);
    delDList(&list);
    delDListItem(&item);
}

void test_DList_insert_after_null_list()
{
    DListItem *curItem = newDListItem(NULL, NULL, NULL);
    DListItem *newItem = newDListItem(NULL, NULL, NULL);
    alib_error err = DList_insert_after(NULL, curItem, newItem);
    assert(err == ALIB_BAD_ARG);
    delDListItem(&curItem);
    delDListItem(&newItem);
}

void test_DList_insert_after_null_curItem()
{
    DList *list = newDList();
    DListItem *newItem = newDListItem(NULL, NULL, NULL);
    alib_error err = DList_insert_after(list, NULL, newItem);
    assert(err == ALIB_BAD_ARG);
    delDList(&list);
    delDListItem(&newItem);
}

void test_DList_insert_after_curItem_wrong_parent()
{
    DList *list = newDList();
    DListItem *curItem = newDListItem(NULL, NULL, NULL);
    DListItem *newItem = newDListItem(NULL, NULL, NULL);
    alib_error err = DList_insert_after(list, curItem, newItem);
    assert(err == ALIB_BAD_ARG);
    delDList(&list);
    delDListItem(&curItem);
    delDListItem(&newItem);
}

void test_DList_insert_after_null_newItem()
{
    DList *list = newDList();
    DListItem *curItem = newDListItem(NULL, NULL, NULL);
    curItem->base.parent = (ListItem *)list;
    alib_error err = DList_insert_after(list, curItem, NULL);
    assert(err == ALIB_BAD_ARG);
    delDList(&list);
    delDListItem(&curItem);
}

void test_DList_insert_after_newItem_with_parent()
{
    DList *list = newDList();
    DListItem *curItem = newDListItem(NULL, NULL, NULL);
    DListItem *newItem = newDListItem(NULL, NULL, NULL);
    curItem->base.parent = (ListItem *)list;
    newItem->base.parent = (ListItem *)list;
    alib_error err = DList_insert_after(list, curItem, newItem);
    assert(err == ALIB_BAD_ARG);
    delDList(&list);
    delDListItem(&curItem);
    delDListItem(&newItem);
}

void test_DList_insert_before_null_list()
{
    DListItem *curItem = newDListItem(NULL, NULL, NULL);
    DListItem *newItem = newDListItem(NULL, NULL, NULL);
    alib_error err = DList_insert_before(NULL, curItem, newItem);
    assert(err == ALIB_BAD_ARG);
    delDListItem(&curItem);
    delDListItem(&newItem);
}

void test_DList_insert_before_null_curItem()
{
    DList *list = newDList();
    DListItem *newItem = newDListItem(NULL, NULL, NULL);
    alib_error err = DList_insert_before(list, NULL, newItem);
    assert(err == ALIB_BAD_ARG);
    delDList(&list);
    delDListItem(&newItem);
}

void test_DList_insert_before_curItem_wrong_parent()
{
    DList *list = newDList();
    DListItem *curItem = newDListItem(NULL, NULL, NULL);
    DListItem *newItem = newDListItem(NULL, NULL, NULL);
    alib_error err = DList_insert_before(list, curItem, newItem);
    assert(err == ALIB_BAD_ARG);
    delDList(&list);
    delDListItem(&curItem);
    delDListItem(&newItem);
}

void test_DList_insert_before_null_newItem()
{
    DList *list = newDList();
    DListItem *curItem = newDListItem(NULL, NULL, NULL);
    curItem->base.parent = (ListItem *)list;
    alib_error err = DList_insert_before(list, curItem, NULL);
    assert(err == ALIB_BAD_ARG);
    delDList(&list);
    delDListItem(&curItem);
}

void test_DList_insert_before_newItem_with_parent()
{
    DList *list = newDList();
    DListItem *curItem = newDListItem(NULL, NULL, NULL);
    DListItem *newItem = newDListItem(NULL, NULL, NULL);
    curItem->base.parent = (ListItem *)list;
    newItem->base.parent = (ListItem *)list;
    alib_error err = DList_insert_before(list, curItem, newItem);
    assert(err == ALIB_BAD_ARG);
    delDList(&list);
    delDListItem(&curItem);
    delDListItem(&newItem);
}

void test_DList_remove_null_list()
{
    alib_error err = DList_remove(NULL, 0);
    assert(err == ALIB_BAD_ARG);
}

void test_DList_remove_out_of_bounds_index()
{
    DList *list = newDList();
    alib_error err = DList_remove(list, 1);
    assert(err == ALIB_BAD_ARG);
    delDList(&list);
}

void test_DList_remove_lsafe_null_list()
{
    alib_error err = DList_remove_lsafe(NULL, 0);
    assert(err == ALIB_BAD_ARG);
}

void test_DList_remove_lsafe_out_of_bounds_index()
{
    DList *list = newDList();
    alib_error err = DList_remove_lsafe(list, 1);
    assert(err == ALIB_BAD_ARG);
    delDList(&list);
}

void test_DList_remove_count_null_list()
{
    alib_error err = DList_remove_count(NULL, 0, 1);
    assert(err == ALIB_BAD_ARG);
}

void test_DList_remove_count_out_of_bounds_index()
{
    DList *list = newDList();
    alib_error err = DList_remove_count(list, 1, 1);
    assert(err == ALIB_BAD_ARG);
    delDList(&list);
}

void test_DList_remove_count_lsafe_null_list()
{
    alib_error err = DList_remove_count_lsafe(NULL, 0, 1);
    assert(err == ALIB_BAD_ARG);
}

void test_DList_remove_count_lsafe_out_of_bounds_index()
{
    DList *list = newDList();
    alib_error err = DList_remove_count_lsafe(list, 1, 1);
    assert(err == ALIB_BAD_ARG);
    delDList(&list);
}

void test_DList_remove_item_null_list()
{
    DListItem *item = newDListItem(NULL, NULL, NULL);
    alib_error err = DList_remove_item(NULL, item);
    assert(err == ALIB_BAD_ARG);
    delDListItem(&item);
}

void test_DList_remove_item_wrong_parent()
{
    DList *list = newDList();
    DListItem *item = newDListItem(NULL, NULL, NULL);
    alib_error err = DList_remove_item(list, item);
    assert(err == ALIB_BAD_ARG);
    delDList(&list);
    delDListItem(&item);
}

void test_DList_remove_item_lsafe_null_list()
{
    DListItem *item = newDListItem(NULL, NULL, NULL);
    alib_error err = DList_remove_item_lsafe(NULL, item);
    assert(err == ALIB_BAD_ARG);
    delDListItem(&item);
}

void test_DList_remove_item_lsafe_wrong_parent()
{
    DList *list = newDList();
    DListItem *item = newDListItem(NULL, NULL, NULL);
    alib_error err = DList_remove_item_lsafe(list, item);
    assert(err == ALIB_BAD_ARG);
    delDList(&list);
    delDListItem(&item);
}

void test_DList_remove_item_count_null_list()
{
    DListItem *item = newDListItem(NULL, NULL, NULL);
    alib_error err = DList_remove_item_count(NULL, item, 1);
    assert(err == ALIB_BAD_ARG);
    delDListItem(&item);
}

void test_DList_remove_item_count_wrong_parent()
{
    DList *list = newDList();
    DListItem *item = newDListItem(NULL, NULL, NULL);
    alib_error err = DList_remove_item_count(list, item, 1);
    assert(err == ALIB_BAD_ARG);
    delDList(&list);
    delDListItem(&item);
}

void test_DList_remove_item_count_lsafe_null_list()
{
    DListItem *item = newDListItem(NULL, NULL, NULL);
    alib_error err = DList_remove_item_count_lsafe(NULL, item, 1);
    assert(err == ALIB_BAD_ARG);
    delDListItem(&item);
}

void test_DList_remove_item_count_lsafe_wrong_parent()
{
    DList *list = newDList();
    DListItem *item = newDListItem(NULL, NULL, NULL);
    alib_error err = DList_remove_item_count_lsafe(list, item, 1);
    assert(err == ALIB_BAD_ARG);
    delDList(&list);
    delDListItem(&item);
}

void test_DList_pull_out_null_list()
{
    DListItem *item = DList_pull_out(NULL, 0);
    assert(item == NULL);
}

void test_DList_pull_out_out_of_bounds_index()
{
    DList *list = newDList();
    DListItem *item = DList_pull_out(list, 1);
    assert(item == NULL);
    delDList(&list);
}

void test_DList_pull_out_count_null_list()
{
    DListItem *item = DList_pull_out_count(NULL, 0, 1);
    assert(item == NULL);
}

void test_DList_pull_out_count_out_of_bounds_index()
{
    DList *list = newDList();
    DListItem *item = DList_pull_out_count(list, 1, 1);
    assert(item == NULL);
    delDList(&list);
}

void test_DList_get_null_list()
{
    ListItem *item = DList_get(NULL, 0);
    assert(item == NULL);
}

void test_DList_get_out_of_bounds_index()
{
    DList *list = newDList();
    ListItem *item = DList_get(list, 1);
    assert(item == NULL);
    delDList(&list);
}

void test_DList_get_by_value_null_list()
{
    int value = 10;
    ListItem *item = DList_get_by_value(NULL, &value);
    assert(item == NULL);
}

void test_DList_get_by_value_not_found()
{
    DList *list = newDList();
    int value = 10;
    ListItem *item = DList_get_by_value(list, &value);
    assert(item == NULL);
    delDList(&list);
}

void test_DList_remove_out_of_bounds()
{
    DList* list = newDList();
    int value1 = 10;
    DListItem* item1 = newDListItem(&value1, NULL, NULL);
    DList_push_back(list, item1);
    assert(DList_get_count(list) == 1);

    DList_remove(list, -1); // Remove at negative index
    assert(DList_get_count(list) == 1);
    assert(DList_get_begin(list) == item1);
    assert(DList_get_end(list) == item1);

    DList_remove(list, 1); // Remove at index beyond end
    assert(DList_get_count(list) == 1);
    assert(DList_get_begin(list) == item1);
    assert(DList_get_end(list) == item1);

    delDList(&list);
}

int main() {
    test_newDList();
    test_DList_push_back_null_list();
    test_DList_push_back_null_item();
    test_DList_push_back_item_with_parent();
    test_DList_push_forward_null_list();
    test_DList_push_forward_null_item();
    test_DList_push_forward_item_with_parent();
    test_DList_insert_null_list();
    test_DList_insert_null_item();
    test_DList_insert_item_with_parent();
    test_DList_insert_after_null_list();
    test_DList_insert_after_null_curItem();
    test_DList_insert_after_curItem_wrong_parent();
    test_DList_insert_after_null_newItem();
    test_DList_insert_after_newItem_with_parent();
    test_DList_insert_before_null_list();
    test_DList_insert_before_null_curItem();
    test_DList_insert_before_curItem_wrong_parent();
    test_DList_insert_before_null_newItem();
    test_DList_insert_before_newItem_with_parent();
    test_DList_remove_null_list();
    test_DList_remove_out_of_bounds_index();
    test_DList_remove_lsafe_null_list();
    test_DList_remove_lsafe_out_of_bounds_index();
    test_DList_remove_count_null_list();
    test_DList_remove_count_out_of_bounds_index();
    test_DList_remove_count_lsafe_null_list();
    test_DList_remove_count_lsafe_out_of_bounds_index();
    test_DList_remove_item_null_list();
    test_DList_remove_item_wrong_parent();
    test_DList_remove_item_lsafe_null_list();
    test_DList_remove_item_lsafe_wrong_parent();
    test_DList_remove_item_count_null_list();
    test_DList_remove_item_count_wrong_parent();
    test_DList_remove_item_count_lsafe_null_list();
    test_DList_remove_item_count_lsafe_wrong_parent();
    test_DList_pull_out_null_list();
    test_DList_pull_out_out_of_bounds_index();
    test_DList_pull_out_count_null_list();
    test_DList_pull_out_count_out_of_bounds_index();
    test_DList_get_null_list();
    test_DList_get_out_of_bounds_index();
    test_DList_get_by_value_null_list();
    test_DList_get_by_value_not_found();
    test_DList_remove_out_of_bounds();

    return 0;
}
