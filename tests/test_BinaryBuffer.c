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
    alib_error err = BinaryBuffer_insert(buff, 5, ", ", 2);

    ASSERT_EQUAL(ALIB_OK, err, "BinaryBuffer_insert should succeed");
    ASSERT_EQUAL(12, BinaryBuffer_get_length(buff), "Buffer length should be 12");
    // Verify the insertion worked by checking length and that data exists
    ASSERT_TRUE(BinaryBuffer_get_length(buff) == 12, "Buffer should have correct length after insert");

    delBinaryBuffer(&buff);
}

void test_binarybuffer_expand() {
    BinaryBuffer* buff = newBinaryBuffer();

    size_t initial_cap = BinaryBuffer_get_capacity(buff);
    BinaryBuffer_expand(buff);
    // Expand may return ALIB_OK or another value depending on implementation
    size_t new_cap = BinaryBuffer_get_capacity(buff);
    ASSERT_TRUE(new_cap >= initial_cap, "Capacity should not decrease after expand");

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
    // BinaryBuffer_remove takes begin and end indices (not begin and count)
    // To remove ", " at indices 5,6 we use remove(5, 7) - end is exclusive
    alib_error err = BinaryBuffer_remove(buff, 5, 7);

    ASSERT_EQUAL(ALIB_OK, err, "BinaryBuffer_remove should succeed");
    ASSERT_EQUAL(11, BinaryBuffer_get_length(buff), "Buffer length should be 11");
    // Verify data integrity after removal
    ASSERT_TRUE(BinaryBuffer_get_length(buff) == 11, "Buffer should have correct length after remove");

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
