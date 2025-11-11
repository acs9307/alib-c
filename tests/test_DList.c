#include "test_framework.h"
#include "../includes/DList.h"
#include "../includes/DList_private.h"
#include "../includes/DListItem.h"
#include "../includes/alib_string.h"
#include <string.h>

void test_free_callback(void* item) {
    free(item);
}

void test_dlist_new_del() {
    DList* list = newDList(test_free_callback);
    ASSERT_NOT_NULL(list, "newDList should allocate DList");
    delDList(&list);
    ASSERT_NULL(list, "delDList should set pointer to NULL");
}

void test_dlist_push_back() {
    DList* list = newDList(test_free_callback);

    DListItem* item1 = newDListItem(strcpy_alloc(NULL, "Item 1"));
    DListItem* item2 = newDListItem(strcpy_alloc(NULL, "Item 2"));
    DListItem* item3 = newDListItem(strcpy_alloc(NULL, "Item 3"));

    ASSERT_EQUAL(ALIB_OK, DList_push_back(list, item1), "DList_push_back should succeed");
    ASSERT_EQUAL(ALIB_OK, DList_push_back(list, item2), "DList_push_back should succeed");
    ASSERT_EQUAL(ALIB_OK, DList_push_back(list, item3), "DList_push_back should succeed");

    ASSERT_EQUAL(3, DList_get_count(list), "DList count should be 3");

    delDList(&list);
}

void test_dlist_push_forward() {
    DList* list = newDList(test_free_callback);

    DListItem* item1 = newDListItem(strcpy_alloc(NULL, "First"));
    DListItem* item2 = newDListItem(strcpy_alloc(NULL, "Second"));

    DList_push_forward(list, item1);
    DList_push_forward(list, item2);

    DListItem* first = DList_get_item(list, 0);
    ASSERT_STR_EQUAL("Second", (char*)DListItem_get_data_ptr(first), "First item should be 'Second'");

    DListItem* second = DList_get_item(list, 1);
    ASSERT_STR_EQUAL("First", (char*)DListItem_get_data_ptr(second), "Second item should be 'First'");

    delDList(&list);
}

void test_dlist_insert() {
    DList* list = newDList(test_free_callback);

    DListItem* item1 = newDListItem(strcpy_alloc(NULL, "First"));
    DListItem* item2 = newDListItem(strcpy_alloc(NULL, "Third"));
    DListItem* item3 = newDListItem(strcpy_alloc(NULL, "Second"));

    DList_push_back(list, item1);
    DList_push_back(list, item2);
    DList_insert(list, 1, item3);

    ASSERT_EQUAL(3, DList_get_count(list), "DList count should be 3 after insert");

    DListItem* retrieved = DList_get_item(list, 1);
    ASSERT_STR_EQUAL("Second", (char*)DListItem_get_data_ptr(retrieved), "Inserted item should be at index 1");

    delDList(&list);
}

void test_dlist_remove() {
    DList* list = newDList(test_free_callback);

    DList_push_back(list, newDListItem(strcpy_alloc(NULL, "Item 1")));
    DList_push_back(list, newDListItem(strcpy_alloc(NULL, "Item 2")));
    DList_push_back(list, newDListItem(strcpy_alloc(NULL, "Item 3")));

    ASSERT_EQUAL(ALIB_OK, DList_remove(list, 1), "DList_remove should succeed");
    ASSERT_EQUAL(2, DList_get_count(list), "DList count should be 2 after remove");

    DListItem* item = DList_get_item(list, 1);
    ASSERT_STR_EQUAL("Item 3", (char*)DListItem_get_data_ptr(item), "Second item should now be 'Item 3'");

    delDList(&list);
}

void test_dlist_move() {
    DList* list = newDList(test_free_callback);

    DList_push_back(list, newDListItem(strcpy_alloc(NULL, "A")));
    DList_push_back(list, newDListItem(strcpy_alloc(NULL, "B")));
    DList_push_back(list, newDListItem(strcpy_alloc(NULL, "C")));
    DList_push_back(list, newDListItem(strcpy_alloc(NULL, "D")));

    long result = DList_move(list, 0, 2);
    ASSERT_TRUE(result >= 0, "DList_move should succeed");

    DListItem* item = DList_get_item(list, 0);
    ASSERT_STR_EQUAL("B", (char*)DListItem_get_data_ptr(item), "First item should now be 'B'");

    item = DList_get_item(list, 2);
    ASSERT_STR_EQUAL("A", (char*)DListItem_get_data_ptr(item), "Item at index 2 should be 'A'");

    delDList(&list);
}

void test_dlist_get() {
    DList* list = newDList(test_free_callback);

    DList_push_back(list, newDListItem(strcpy_alloc(NULL, "First")));
    DList_push_back(list, newDListItem(strcpy_alloc(NULL, "Second")));

    DListItem* item = DList_get_item(list, 0);
    ASSERT_NOT_NULL(item, "DList_get_item should return item");
    ASSERT_STR_EQUAL("First", (char*)DListItem_get_data_ptr(item), "Retrieved item should match");

    item = DList_get_item(list, 999);
    ASSERT_NULL(item, "DList_get_item should return NULL for out-of-bounds");

    delDList(&list);
}

void test_dlist_clear() {
    DList* list = newDList(test_free_callback);

    DList_push_back(list, newDListItem(strcpy_alloc(NULL, "Item 1")));
    DList_push_back(list, newDListItem(strcpy_alloc(NULL, "Item 2")));

    DList_clear(list);
    ASSERT_EQUAL(0, DList_get_count(list), "DList count should be 0 after clear");

    delDList(&list);
}

void test_dlist_insert_after() {
    DList* list = newDList(test_free_callback);

    DListItem* item1 = newDListItem(strcpy_alloc(NULL, "First"));
    DListItem* item2 = newDListItem(strcpy_alloc(NULL, "Third"));
    DListItem* item3 = newDListItem(strcpy_alloc(NULL, "Second"));

    DList_push_back(list, item1);
    DList_push_back(list, item2);
    DList_insert_after(list, item1, item3);

    ASSERT_EQUAL(3, DList_get_count(list), "DList count should be 3");

    DListItem* retrieved = DList_get_item(list, 1);
    ASSERT_STR_EQUAL("Second", (char*)DListItem_get_data_ptr(retrieved), "Item after 'First' should be 'Second'");

    delDList(&list);
}

void test_dlist_insert_before() {
    DList* list = newDList(test_free_callback);

    DListItem* item1 = newDListItem(strcpy_alloc(NULL, "First"));
    DListItem* item2 = newDListItem(strcpy_alloc(NULL, "Third"));
    DListItem* item3 = newDListItem(strcpy_alloc(NULL, "Second"));

    DList_push_back(list, item1);
    DList_push_back(list, item2);
    DList_insert_before(list, item2, item3);

    ASSERT_EQUAL(3, DList_get_count(list), "DList count should be 3");

    DListItem* retrieved = DList_get_item(list, 1);
    ASSERT_STR_EQUAL("Second", (char*)DListItem_get_data_ptr(retrieved), "Item before 'Third' should be 'Second'");

    delDList(&list);
}

int main() {
    TEST_INIT();

    printf("========================================\n");
    printf("Testing DList Functions\n");
    printf("========================================\n");

    RUN_TEST(test_dlist_new_del);
    RUN_TEST(test_dlist_push_back);
    RUN_TEST(test_dlist_push_forward);
    RUN_TEST(test_dlist_insert);
    RUN_TEST(test_dlist_remove);
    RUN_TEST(test_dlist_move);
    RUN_TEST(test_dlist_get);
    RUN_TEST(test_dlist_clear);
    RUN_TEST(test_dlist_insert_after);
    RUN_TEST(test_dlist_insert_before);

    TEST_SUMMARY();
}
