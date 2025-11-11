#include "test_framework.h"
#include "../includes/ArrayList.h"
#include "../includes/ArrayList_private.h"
#include "../includes/alib_string.h"
#include <string.h>

// Simple delete callback for testing
void test_free_callback(void* item) {
    free(item);
}

// Simple compare callback
int test_compare_callback(const void* a, const void* b) {
    return strcmp((const char*)a, (const char*)b);
}

void test_arraylist_new_del() {
    ArrayList* list = newArrayList(test_free_callback);
    ASSERT_NOT_NULL(list, "newArrayList should allocate ArrayList");
    delArrayList(&list);
    ASSERT_NULL(list, "delArrayList should set pointer to NULL");
}

void test_arraylist_add() {
    ArrayList* list = newArrayList(test_free_callback);

    char* item1 = strcpy_alloc(NULL, "Item 1");
    char* item2 = strcpy_alloc(NULL, "Item 2");
    char* item3 = strcpy_alloc(NULL, "Item 3");

    ASSERT_TRUE(ArrayList_add(list, item1), "ArrayList_add should succeed");
    ASSERT_TRUE(ArrayList_add(list, item2), "ArrayList_add should succeed for second item");
    ASSERT_TRUE(ArrayList_add(list, item3), "ArrayList_add should succeed for third item");

    ASSERT_EQUAL(3, ArrayList_get_count(list), "ArrayList count should be 3");

    delArrayList(&list);
}

void test_arraylist_get() {
    ArrayList* list = newArrayList(test_free_callback);

    char* item1 = strcpy_alloc(NULL, "First");
    char* item2 = strcpy_alloc(NULL, "Second");

    ArrayList_add(list, item1);
    ArrayList_add(list, item2);

    const void* retrieved = ArrayList_get_by_index(list, 0);
    ASSERT_NOT_NULL(retrieved, "ArrayList_get_by_index should retrieve item at index 0");
    ASSERT_STR_EQUAL("First", (char*)retrieved, "Retrieved item should match");

    retrieved = ArrayList_get_by_index(list, 1);
    ASSERT_STR_EQUAL("Second", (char*)retrieved, "Retrieved item at index 1 should match");

    retrieved = ArrayList_get_by_index(list, 999);
    ASSERT_NULL(retrieved, "ArrayList_get_by_index should return NULL for out-of-bounds index");

    delArrayList(&list);
}

void test_arraylist_remove() {
    ArrayList* list = newArrayList(NULL);  // No auto-free for manual control

    char item1[] = "Item 1";
    char item2[] = "Item 2";
    char item3[] = "Item 3";

    ArrayList_add(list, item1);
    ArrayList_add(list, item2);
    ArrayList_add(list, item3);

    ArrayList_remove_no_free(list, item2);
    ASSERT_EQUAL(2, ArrayList_get_count(list), "ArrayList count should be 2 after remove");

    const void* retrieved = ArrayList_get_by_index(list, 0);
    ASSERT_STR_EQUAL("Item 1", (char*)retrieved, "First item should still be Item 1");

    retrieved = ArrayList_get_by_index(list, 1);
    ASSERT_STR_EQUAL("Item 3", (char*)retrieved, "Second item should now be Item 3");

    delArrayList(&list);
}

void test_arraylist_clear() {
    ArrayList* list = newArrayList(test_free_callback);

    ArrayList_add(list, strcpy_alloc(NULL, "Item 1"));
    ArrayList_add(list, strcpy_alloc(NULL, "Item 2"));
    ArrayList_add(list, strcpy_alloc(NULL, "Item 3"));

    ArrayList_clear(list);
    ASSERT_EQUAL(0, ArrayList_get_count(list), "ArrayList count should be 0 after clear");

    delArrayList(&list);
}

void test_arraylist_find() {
    ArrayList* list = newArrayList(test_free_callback);

    char* item1 = strcpy_alloc(NULL, "Apple");
    char* item2 = strcpy_alloc(NULL, "Banana");
    char* item3 = strcpy_alloc(NULL, "Cherry");

    ArrayList_add(list, item1);
    ArrayList_add(list, item2);
    ArrayList_add(list, item3);

    const char* search_val = "Banana";
    const void* found = ArrayList_find_item_by_value(list, search_val, test_compare_callback);
    ASSERT_NOT_NULL(found, "ArrayList_find_item_by_value should find item");
    ASSERT_STR_EQUAL("Banana", (char*)found, "Found item should be Banana");

    search_val = "Durian";
    found = ArrayList_find_item_by_value(list, search_val, test_compare_callback);
    ASSERT_NULL(found, "ArrayList_find_item_by_value should return NULL for non-existent item");

    delArrayList(&list);
}

void test_arraylist_contains() {
    ArrayList* list = newArrayList(test_free_callback);

    char* item1 = strcpy_alloc(NULL, "Test");
    ArrayList_add(list, item1);

    ASSERT_TRUE(ArrayList_contains(list, "Test", 4), "ArrayList_contains should find existing data");
    ASSERT_FALSE(ArrayList_contains(list, "Nope", 4), "ArrayList_contains should not find non-existent data");

    delArrayList(&list);
}

void test_arraylist_resize() {
    ArrayList* list = newArrayList(test_free_callback);

    size_t initial_cap = ArrayList_get_capacity(list);

    // Add items to force resize
    for (int i = 0; i < 100; i++) {
        char buffer[20];
        snprintf(buffer, sizeof(buffer), "Item %d", i);
        ArrayList_add(list, strcpy_alloc(NULL, buffer));
    }

    size_t new_cap = ArrayList_get_capacity(list);
    ASSERT_TRUE(new_cap > initial_cap, "ArrayList capacity should increase after many adds");
    ASSERT_EQUAL(100, ArrayList_get_count(list), "ArrayList count should be 100");

    delArrayList(&list);
}

void test_arraylist_get_first() {
    ArrayList* list = newArrayList(test_free_callback);

    void* first = ArrayList_get_first_item(list);
    ASSERT_NULL(first, "ArrayList_get_first_item should return NULL for empty list");

    ArrayList_add(list, strcpy_alloc(NULL, "First"));
    ArrayList_add(list, strcpy_alloc(NULL, "Second"));

    first = ArrayList_get_first_item(list);
    ASSERT_NOT_NULL(first, "ArrayList_get_first_item should return item");
    ASSERT_STR_EQUAL("First", (char*)first, "First item should be correct");

    delArrayList(&list);
}

int main() {
    TEST_INIT();

    printf("========================================\n");
    printf("Testing ArrayList Functions\n");
    printf("========================================\n");

    RUN_TEST(test_arraylist_new_del);
    RUN_TEST(test_arraylist_add);
    RUN_TEST(test_arraylist_get);
    RUN_TEST(test_arraylist_remove);
    RUN_TEST(test_arraylist_clear);
    RUN_TEST(test_arraylist_find);
    RUN_TEST(test_arraylist_contains);
    RUN_TEST(test_arraylist_resize);
    RUN_TEST(test_arraylist_get_first);

    TEST_SUMMARY();
}
