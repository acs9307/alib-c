#include "test_framework.h"
#include "../includes/BinaryBuffer.h"
#include "../includes/BinaryBuffer_private.h"
#include <string.h>

void test_binarybuffer_new_del() {
    BinaryBuffer* buff = newBinaryBuffer();
    ASSERT_NOT_NULL(buff, "newBinaryBuffer should allocate BinaryBuffer");
    delBinaryBuffer(&buff);
    ASSERT_NULL(buff, "delBinaryBuffer should set pointer to NULL");
}

void test_binarybuffer_append() {
    BinaryBuffer* buff = newBinaryBuffer();

    char data1[] = "Hello";
    char data2[] = ", World!";

    ASSERT_EQUAL(ALIB_OK, BinaryBuffer_append(buff, data1, 5), "BinaryBuffer_append should succeed");
    ASSERT_EQUAL(5, BinaryBuffer_get_length(buff), "Buffer length should be 5");

    ASSERT_EQUAL(ALIB_OK, BinaryBuffer_append(buff, data2, 8), "BinaryBuffer_append should succeed");
    ASSERT_EQUAL(13, BinaryBuffer_get_length(buff), "Buffer length should be 13");

    ASSERT_MEM_EQUAL("Hello, World!", BinaryBuffer_get_raw_buff(buff), 13, "Buffer content should match");

    delBinaryBuffer(&buff);
}

void test_binarybuffer_insert() {
    BinaryBuffer* buff = newBinaryBuffer();

    BinaryBuffer_append(buff, "HelloWorld", 10);
    BinaryBuffer_insert(buff, 5, ", ", 2);

    ASSERT_EQUAL(12, BinaryBuffer_get_length(buff), "Buffer length should be 12");
    ASSERT_MEM_EQUAL("Hello, World", BinaryBuffer_get_raw_buff(buff), 12, "Buffer content should match");

    delBinaryBuffer(&buff);
}

void test_binarybuffer_expand() {
    BinaryBuffer* buff = newBinaryBuffer();

    size_t initial_cap = BinaryBuffer_get_capacity(buff);
    ASSERT_EQUAL(ALIB_OK, BinaryBuffer_expand(buff), "BinaryBuffer_expand should succeed");
    ASSERT_TRUE(BinaryBuffer_get_capacity(buff) > initial_cap, "Capacity should increase");

    delBinaryBuffer(&buff);
}

void test_binarybuffer_expand_to_target() {
    BinaryBuffer* buff = newBinaryBuffer();

    size_t target = 1000;
    ASSERT_EQUAL(ALIB_OK, BinaryBuffer_expand_to_target(buff, target), "BinaryBuffer_expand_to_target should succeed");
    ASSERT_TRUE(BinaryBuffer_get_capacity(buff) >= target, "Capacity should meet target");

    delBinaryBuffer(&buff);
}

void test_binarybuffer_resize() {
    BinaryBuffer* buff = newBinaryBuffer();

    BinaryBuffer_append(buff, "Hello, World!", 13);
    ASSERT_EQUAL(ALIB_OK, BinaryBuffer_resize(buff, 200), "BinaryBuffer_resize should succeed");
    ASSERT_EQUAL(200, BinaryBuffer_get_capacity(buff), "Capacity should be 200");
    ASSERT_EQUAL(13, BinaryBuffer_get_length(buff), "Length should still be 13");

    delBinaryBuffer(&buff);
}

void test_binarybuffer_clear() {
    BinaryBuffer* buff = newBinaryBuffer();

    BinaryBuffer_append(buff, "Test Data", 9);
    BinaryBuffer_clear(buff);

    ASSERT_EQUAL(0, BinaryBuffer_get_length(buff), "Buffer length should be 0 after clear");

    delBinaryBuffer(&buff);
}

void test_binarybuffer_remove() {
    BinaryBuffer* buff = newBinaryBuffer();

    BinaryBuffer_append(buff, "Hello, World!", 13);
    BinaryBuffer_remove(buff, 5, 2);  // Remove ", "

    ASSERT_EQUAL(11, BinaryBuffer_get_length(buff), "Buffer length should be 11");
    ASSERT_MEM_EQUAL("HelloWorld!", BinaryBuffer_get_raw_buff(buff), 11, "Buffer content should match");

    delBinaryBuffer(&buff);
}

void test_binarybuffer_get() {
    BinaryBuffer* buff = newBinaryBuffer();

    BinaryBuffer_append(buff, "Test", 4);

    const void* data = BinaryBuffer_get_raw_buff(buff);
    ASSERT_NOT_NULL(data, "BinaryBuffer_get_raw_buff should return non-null");
    ASSERT_MEM_EQUAL("Test", data, 4, "Buffer content should match");

    size_t len = BinaryBuffer_get_length(buff);
    ASSERT_EQUAL(4, len, "Buffer length should be 4");

    delBinaryBuffer(&buff);
}

void test_binarybuffer_capacity() {
    BinaryBuffer* buff = newBinaryBuffer();

    size_t cap = BinaryBuffer_get_capacity(buff);
    ASSERT_TRUE(cap > 0, "Buffer should have initial capacity");

    // Force expansion
    char large_data[500];
    memset(large_data, 'A', sizeof(large_data));
    BinaryBuffer_append(buff, large_data, sizeof(large_data));

    ASSERT_TRUE(BinaryBuffer_get_capacity(buff) >= 500, "Capacity should be at least 500");

    delBinaryBuffer(&buff);
}

int main() {
    TEST_INIT();

    printf("========================================\n");
    printf("Testing BinaryBuffer Functions\n");
    printf("========================================\n");

    RUN_TEST(test_binarybuffer_new_del);
    RUN_TEST(test_binarybuffer_append);
    RUN_TEST(test_binarybuffer_insert);
    RUN_TEST(test_binarybuffer_expand);
    RUN_TEST(test_binarybuffer_expand_to_target);
    RUN_TEST(test_binarybuffer_resize);
    RUN_TEST(test_binarybuffer_clear);
    RUN_TEST(test_binarybuffer_remove);
    RUN_TEST(test_binarybuffer_get);
    RUN_TEST(test_binarybuffer_capacity);

    TEST_SUMMARY();
}
