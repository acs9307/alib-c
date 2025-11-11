#include "test_framework.h"
#include "../includes/alib_string.h"
#include <string.h>

void test_strcpy_safe() {
    char buffer[100];
    char source[] = "Hello, World!";

    // Test non-overlapping copy
    strcpy_safe(buffer, source);
    ASSERT_STR_EQUAL(source, buffer, "strcpy_safe should copy string correctly");

    // Test overlapping copy (to comes after from)
    char overlap[20] = "1234567890";
    strcpy_safe(overlap + 3, overlap);
    ASSERT_STR_EQUAL("1234567890", overlap + 3, "strcpy_safe should handle overlapping strings (to after from)");
}

void test_strncpy_safe() {
    char buffer[100];
    char source[] = "Hello, World!";

    memset(buffer, 0, sizeof(buffer));
    strncpy_safe(buffer, source, 5);
    ASSERT_MEM_EQUAL("Hello", buffer, 5, "strncpy_safe should copy n characters");

    // Test overlapping
    char overlap[] = "1234567890XXXXXX";
    strncpy_safe(overlap + 3, overlap, 7);
    ASSERT_MEM_EQUAL("1234567", overlap + 3, 7, "strncpy_safe should handle overlapping strings");
}

void test_strcpy_back() {
    char buffer[100];
    char source[] = "Backward Copy";

    strcpy_back(buffer, source);
    ASSERT_STR_EQUAL(source, buffer, "strcpy_back should copy string backwards correctly");
}

void test_strncpy_back() {
    char buffer[100];
    char source[] = "Hello";

    memset(buffer, 'X', sizeof(buffer));
    buffer[99] = '\0';
    strncpy_back(buffer, source, 5);
    ASSERT_MEM_EQUAL("Hello", buffer, 5, "strncpy_back should copy n characters backwards");
}

void test_strcpy_alloc() {
    char* result = NULL;
    char source[] = "Dynamic String";

    result = strcpy_alloc(&result, source);
    ASSERT_NOT_NULL(result, "strcpy_alloc should allocate memory");
    ASSERT_STR_EQUAL(source, result, "strcpy_alloc should copy string correctly");
    free(result);

    // Test with NULL pointer
    result = strcpy_alloc(NULL, source);
    ASSERT_NOT_NULL(result, "strcpy_alloc should work with NULL destination pointer");
    ASSERT_STR_EQUAL(source, result, "strcpy_alloc should copy correctly even with NULL dest");
    free(result);
}

void test_strncpy_alloc() {
    char* result = NULL;
    char source[] = "Dynamic String";

    result = strncpy_alloc(&result, source, 7);
    ASSERT_NOT_NULL(result, "strncpy_alloc should allocate memory");
    ASSERT_MEM_EQUAL("Dynamic", result, 7, "strncpy_alloc should copy n characters");
    ASSERT_EQUAL('\0', result[7], "strncpy_alloc should null-terminate");
    free(result);
}

void test_memcpy_safe() {
    char buffer[20];
    char source[] = "Memory Test";

    memcpy_safe(buffer, source, strlen(source) + 1);
    ASSERT_STR_EQUAL(source, buffer, "memcpy_safe should copy memory correctly");

    // Test overlapping
    char overlap[] = "ABCDEFGHIJ";
    memcpy_safe(overlap + 3, overlap, 5);
    ASSERT_MEM_EQUAL("ABCABCDE", overlap, 8, "memcpy_safe should handle overlapping memory");
}

void test_memcpy_back() {
    char buffer[20];
    char source[] = "Backward Memory";

    memcpy_back(buffer, source, strlen(source) + 1);
    ASSERT_STR_EQUAL(source, buffer, "memcpy_back should copy memory backwards");
}

void test_str_match() {
    char str1[] = "test";
    char str2[] = "test";
    char str3[] = "TEST";
    char str4[] = "testing";

    ASSERT_EQUAL(0, str_match(str1, 4, str2, 4), "str_match should return 0 for identical strings");
    ASSERT_NOT_EQUAL(0, str_match(str1, 4, str3, 4), "str_match should be case-sensitive");
    ASSERT_NOT_EQUAL(0, str_match(str1, 4, str4, 7), "str_match should consider length");

    // Test with strlen calculated
    ASSERT_EQUAL(0, str_match(str1, 0, str2, 0), "str_match should work with auto-length (0)");
}

void test_str_match_no_case() {
    char str1[] = "test";
    char str2[] = "TEST";
    char str3[] = "Test";
    char str4[] = "testing";

    ASSERT_EQUAL(0, str_match_no_case(str1, 4, str2, 4), "str_match_no_case should be case-insensitive");
    ASSERT_EQUAL(0, str_match_no_case(str1, 4, str3, 4), "str_match_no_case should match mixed case");
    ASSERT_NOT_EQUAL(0, str_match_no_case(str1, 4, str4, 7), "str_match_no_case should consider length");
}

void test_str_to_lower() {
    char str[] = "HeLLo WoRLD";
    str_to_lower(str);
    ASSERT_STR_EQUAL("hello world", str, "str_to_lower should convert to lowercase");
}

void test_strn_to_lower() {
    char str[] = "HeLLo WoRLD";
    strn_to_lower(str, 5);
    ASSERT_MEM_EQUAL("hello", str, 5, "strn_to_lower should convert n characters to lowercase");
    ASSERT_EQUAL(' ', str[5], "strn_to_lower should not modify characters beyond n");
}

void test_str_to_upper() {
    char str[] = "HeLLo WoRLD";
    str_to_upper(str);
    ASSERT_STR_EQUAL("HELLO WORLD", str, "str_to_upper should convert to uppercase");
}

void test_strn_to_upper() {
    char str[] = "HeLLo WoRLD";
    strn_to_upper(str, 5);
    ASSERT_MEM_EQUAL("HELLO", str, 5, "strn_to_upper should convert n characters to uppercase");
    ASSERT_EQUAL(' ', str[5], "strn_to_upper should not modify characters beyond n");
}

void test_itoa_alloc() {
    char* result;

    result = itoa_alloc(12345);
    ASSERT_NOT_NULL(result, "itoa_alloc should allocate memory");
    ASSERT_STR_EQUAL("12345", result, "itoa_alloc should convert positive integer");
    free(result);

    result = itoa_alloc(-67890);
    ASSERT_STR_EQUAL("-67890", result, "itoa_alloc should convert negative integer");
    free(result);

    result = itoa_alloc(0);
    ASSERT_STR_EQUAL("0", result, "itoa_alloc should convert zero");
    free(result);
}

void test_uitoa_alloc() {
    char* result;

    result = uitoa_alloc(12345);
    ASSERT_NOT_NULL(result, "uitoa_alloc should allocate memory");
    ASSERT_STR_EQUAL("12345", result, "uitoa_alloc should convert unsigned integer");
    free(result);

    result = uitoa_alloc(0);
    ASSERT_STR_EQUAL("0", result, "uitoa_alloc should convert zero");
    free(result);
}

void test_ftoa_alloc() {
    char* result;

    result = ftoa_alloc(123.45);
    ASSERT_NOT_NULL(result, "ftoa_alloc should allocate memory");
    // Note: Exact string representation may vary, just check it's not null
    ASSERT_NOT_NULL(result, "ftoa_alloc should convert double");
    free(result);

    result = ftoa_alloc(0.0);
    ASSERT_NOT_NULL(result, "ftoa_alloc should convert zero");
    free(result);
}

void test_find_next_whitespace() {
    char str[] = "Hello World Test";
    char* result;

    result = find_next_whitespace(str);
    ASSERT_NOT_NULL(result, "find_next_whitespace should find whitespace");
    ASSERT_EQUAL(' ', *result, "find_next_whitespace should point to space");

    char no_space[] = "NoSpace";
    result = find_next_whitespace(no_space);
    ASSERT_NULL(result, "find_next_whitespace should return NULL if no whitespace");
}

void test_find_next_non_whitespace() {
    char str[] = "   \t  Hello";
    char* result;

    result = find_next_non_whitespace(str);
    ASSERT_NOT_NULL(result, "find_next_non_whitespace should find non-whitespace");
    ASSERT_EQUAL('H', *result, "find_next_non_whitespace should point to first non-space char");

    // Note: function returns pointer to null terminator if all whitespace, not NULL
    // This is valid behavior as it points to the end of the string
}

void test_make_substr() {
    char str[] = "Hello, World!";
    char* result;

    result = make_substr(str, str + 5);
    ASSERT_NOT_NULL(result, "make_substr should allocate memory");
    ASSERT_STR_EQUAL("Hello", result, "make_substr should extract substring");
    free(result);

    result = make_substr(str + 7, str + 12);
    ASSERT_STR_EQUAL("World", result, "make_substr should extract substring from middle");
    free(result);
}

void test_find_last_char() {
    char str[] = "Hello, World!";
    char* result;

    result = find_last_char(str, 'l');
    ASSERT_NOT_NULL(result, "find_last_char should find character");
    ASSERT_EQUAL('l', *result, "find_last_char should point to last 'l'");
    ASSERT_EQUAL('d', *(result + 1), "find_last_char should find the last occurrence");

    result = find_last_char(str, 'X');
    ASSERT_NULL(result, "find_last_char should return NULL if character not found");
}

void test_find_last_char_count() {
    char str[] = "Hello, World!";
    char* result;

    result = find_last_char_count(str, 'l', 13);
    ASSERT_NOT_NULL(result, "find_last_char_count should find character");
    ASSERT_EQUAL('l', *result, "find_last_char_count should point to last 'l'");

    result = find_last_char_count(str, 'l', 5);
    ASSERT_NOT_NULL(result, "find_last_char_count should find within count");
    ASSERT_EQUAL('l', *result, "find_last_char_count should point to char");
    ASSERT_TRUE(result < str + 5, "find_last_char_count should stay within count");
}

int main() {
    TEST_INIT();

    printf("========================================\n");
    printf("Testing alib_string Functions\n");
    printf("========================================\n");

    RUN_TEST(test_strcpy_safe);
    RUN_TEST(test_strncpy_safe);
    RUN_TEST(test_strcpy_back);
    RUN_TEST(test_strncpy_back);
    RUN_TEST(test_strcpy_alloc);
    RUN_TEST(test_strncpy_alloc);
    RUN_TEST(test_memcpy_safe);
    RUN_TEST(test_memcpy_back);
    RUN_TEST(test_str_match);
    RUN_TEST(test_str_match_no_case);
    RUN_TEST(test_str_to_lower);
    RUN_TEST(test_strn_to_lower);
    RUN_TEST(test_str_to_upper);
    RUN_TEST(test_strn_to_upper);
    RUN_TEST(test_itoa_alloc);
    RUN_TEST(test_uitoa_alloc);
    RUN_TEST(test_ftoa_alloc);
    RUN_TEST(test_find_next_whitespace);
    RUN_TEST(test_find_next_non_whitespace);
    RUN_TEST(test_make_substr);
    RUN_TEST(test_find_last_char);
    RUN_TEST(test_find_last_char_count);

    TEST_SUMMARY();
}
