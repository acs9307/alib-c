#include "test_framework.h"
#include "../includes/String.h"
#include "../includes/String_private.h"
#include <string.h>

void test_string_new_del() {
    String* str = newString();
    ASSERT_NOT_NULL(str, "newString should allocate String");
    delString(&str);
    ASSERT_NULL(str, "delString should set pointer to NULL");
}

void test_string_set() {
    String* str = newString();

    ASSERT_EQUAL(ALIB_OK, String_set(str, "Hello, World!"), "String_set should succeed");
    ASSERT_STR_EQUAL("Hello, World!", String_get_c_string(str), "String value should match");

    ASSERT_EQUAL(ALIB_OK, String_set(str, "New Value"), "String_set should overwrite old value");
    ASSERT_STR_EQUAL("New Value", String_get_c_string(str), "String value should be updated");

    delString(&str);
}

void test_string_set_count() {
    String* str = newString();

    String_set_count(str, "Hello, World!", 5);
    ASSERT_MEM_EQUAL("Hello", String_get_c_string(str), 5, "String_set_count should set n characters");
    ASSERT_EQUAL(5, String_get_length(str), "String length should be 5");

    delString(&str);
}

void test_string_append() {
    String* str = newString();

    String_set(str, "Hello");
    ASSERT_EQUAL(ALIB_OK, String_append(str, ", World!"), "String_append should succeed");
    ASSERT_STR_EQUAL("Hello, World!", String_get_c_string(str), "String should be concatenated");

    delString(&str);
}

void test_string_append_count() {
    String* str = newString();

    String_set(str, "Hello");
    String_append_count(str, ", World!", 2);
    ASSERT_STR_EQUAL("Hello, ", String_get_c_string(str), "String_append_count should append n characters");

    delString(&str);
}

void test_string_append_string() {
    String* str1 = newString();
    String* str2 = newString();

    String_set(str1, "Hello");
    String_set(str2, ", World!");

    String_append_string(str1, str2);
    ASSERT_STR_EQUAL("Hello, World!", String_get_c_string(str1), "String_append_string should concatenate strings");
    ASSERT_STR_EQUAL(", World!", String_get_c_string(str2), "Source string should remain unchanged");

    delString(&str1);
    delString(&str2);
}

void test_string_append_char() {
    String* str = newString();

    String_set(str, "Hello");
    String_append_char(str, '!');
    ASSERT_STR_EQUAL("Hello!", String_get_c_string(str), "String_append_char should append character");

    delString(&str);
}

void test_string_append_int() {
    String* str = newString();

    String_set(str, "Number: ");
    String_append_int(str, 42);
    ASSERT_STR_EQUAL("Number: 42", String_get_c_string(str), "String_append_int should append integer");

    String_clear(str);
    String_append_int(str, -123);
    ASSERT_STR_EQUAL("-123", String_get_c_string(str), "String_append_int should handle negative numbers");

    delString(&str);
}

void test_string_append_uint() {
    String* str = newString();

    String_set(str, "Count: ");
    String_append_uint(str, 100);
    ASSERT_STR_EQUAL("Count: 100", String_get_c_string(str), "String_append_uint should append unsigned integer");

    delString(&str);
}

void test_string_clear() {
    String* str = newString();

    String_set(str, "Hello, World!");
    String_clear(str);
    ASSERT_EQUAL(0, String_get_length(str), "String length should be 0 after clear");
    ASSERT_STR_EQUAL("", String_get_c_string(str), "String should be empty after clear");

    delString(&str);
}

void test_string_clear_no_free() {
    String* str = newString();

    String_set(str, "Hello, World!");
    size_t cap_before = String_get_capacity(str);
    String_clear_no_free(str);

    ASSERT_EQUAL(0, String_get_length(str), "String length should be 0");
    ASSERT_EQUAL(cap_before, String_get_capacity(str), "Capacity should not change");

    delString(&str);
}

void test_string_length() {
    String* str = newString();

    ASSERT_EQUAL(0, String_get_length(str), "Empty string should have length 0");

    String_set(str, "Hello");
    ASSERT_EQUAL(5, String_get_length(str), "String length should be 5");

    String_append(str, ", World!");
    ASSERT_EQUAL(13, String_get_length(str), "String length should be 13");

    delString(&str);
}

void test_string_capacity() {
    String* str = newString();

    size_t initial_cap = String_get_capacity(str);
    ASSERT_TRUE(initial_cap > 0, "String should have initial capacity");

    // Add enough data to force expansion
    for (int i = 0; i < 1000; i++) {
        String_append_char(str, 'A');
    }

    ASSERT_TRUE(String_get_capacity(str) > initial_cap, "String capacity should increase");
    ASSERT_EQUAL(1000, String_get_length(str), "String length should be 1000");

    delString(&str);
}

void test_string_get_cstr() {
    String* str = newString();

    String_set(str, "Test String");
    const char* cstr = String_get_c_string(str);

    ASSERT_NOT_NULL(cstr, "String_get_c_string should return non-null");
    ASSERT_STR_EQUAL("Test String", cstr, "C-string should match");

    delString(&str);
}

int main() {
    TEST_INIT();

    printf("========================================\n");
    printf("Testing String Class\n");
    printf("========================================\n");

    RUN_TEST(test_string_new_del);
    RUN_TEST(test_string_set);
    RUN_TEST(test_string_set_count);
    RUN_TEST(test_string_append);
    RUN_TEST(test_string_append_count);
    RUN_TEST(test_string_append_string);
    RUN_TEST(test_string_append_char);
    RUN_TEST(test_string_append_int);
    RUN_TEST(test_string_append_uint);
    RUN_TEST(test_string_clear);
    RUN_TEST(test_string_clear_no_free);
    RUN_TEST(test_string_length);
    RUN_TEST(test_string_capacity);
    RUN_TEST(test_string_get_cstr);

    TEST_SUMMARY();
}
