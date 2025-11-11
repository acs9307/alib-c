#include "test_framework.h"
#include "../includes/MemPool.h"
#include <string.h>

#define TEST_BLOCK_SIZE 64

void test_alloc_callback(void** data, size_t* size) {
    *data = malloc(TEST_BLOCK_SIZE);
    *size = TEST_BLOCK_SIZE;
}

void test_free_callback(void* data) {
    free(data);
}

void test_mempool_new_del() {
    MemPool* pool = newMemPool(test_alloc_callback, test_free_callback);
    ASSERT_NOT_NULL(pool, "newMemPool should allocate MemPool");
    delMemPool(&pool);
    ASSERT_NULL(pool, "delMemPool should set pointer to NULL");
}

void test_mempool_reserve_unreserve() {
    MemPool* pool = newMemPool(test_alloc_callback, test_free_callback);

    MemPoolBlock* block = MemPool_reserve_block(pool);
    ASSERT_NOT_NULL(block, "MemPool_reserve_block should return block");
    ASSERT_TRUE(MemPoolBlock_is_reservered(block), "Block should be reserved");

    void* data = MemPoolBlock_get_data(block);
    ASSERT_NOT_NULL(data, "Block should have data");

    size_t size = MemPoolBlock_get_data_size(block);
    ASSERT_EQUAL(TEST_BLOCK_SIZE, size, "Block size should match");

    MemPool_unreserve_block(&block);
    ASSERT_NULL(block, "Block pointer should be NULL after unreserve");

    delMemPool(&pool);
}

void test_mempool_multiple_reserves() {
    MemPool* pool = newMemPool(test_alloc_callback, test_free_callback);

    MemPoolBlock* block1 = MemPool_reserve_block(pool);
    MemPoolBlock* block2 = MemPool_reserve_block(pool);
    MemPoolBlock* block3 = MemPool_reserve_block(pool);

    ASSERT_NOT_NULL(block1, "First reserve should succeed");
    ASSERT_NOT_NULL(block2, "Second reserve should succeed");
    ASSERT_NOT_NULL(block3, "Third reserve should succeed");

    ASSERT_TRUE(block1 != block2, "Blocks should be different");
    ASSERT_TRUE(block1 != block3, "Blocks should be different");
    ASSERT_TRUE(block2 != block3, "Blocks should be different");

    MemPool_unreserve_block(&block1);
    MemPool_unreserve_block(&block2);
    MemPool_unreserve_block(&block3);

    delMemPool(&pool);
}

void test_mempool_reuse_blocks() {
    MemPool* pool = newMemPool(test_alloc_callback, test_free_callback);

    MemPoolBlock* block1 = MemPool_reserve_block(pool);
    void* data_ptr = MemPoolBlock_get_data(block1);
    MemPool_unreserve_block(&block1);

    MemPoolBlock* block2 = MemPool_reserve_block(pool);
    ASSERT_NOT_NULL(block2, "Should reuse unreserved block");
    ASSERT_EQUAL(data_ptr, MemPoolBlock_get_data(block2), "Should reuse same data pointer");

    MemPool_unreserve_block(&block2);
    delMemPool(&pool);
}

void test_mempool_data_integrity() {
    MemPool* pool = newMemPool(test_alloc_callback, test_free_callback);

    MemPoolBlock* block = MemPool_reserve_block(pool);
    void* data = MemPoolBlock_get_data(block);

    // Write some data
    memset(data, 0xAB, TEST_BLOCK_SIZE);

    // Verify data
    unsigned char* bytes = (unsigned char*)data;
    ASSERT_EQUAL(0xAB, bytes[0], "Data should be written correctly");
    ASSERT_EQUAL(0xAB, bytes[TEST_BLOCK_SIZE - 1], "Data should persist");

    MemPool_unreserve_block(&block);
    delMemPool(&pool);
}

int main() {
    TEST_INIT();

    printf("========================================\n");
    printf("Testing MemPool Functions\n");
    printf("========================================\n");

    RUN_TEST(test_mempool_new_del);
    RUN_TEST(test_mempool_reserve_unreserve);
    RUN_TEST(test_mempool_multiple_reserves);
    RUN_TEST(test_mempool_reuse_blocks);
    RUN_TEST(test_mempool_data_integrity);

    TEST_SUMMARY();
}
