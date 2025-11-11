#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Simple test framework for alib-c */

static int test_count = 0;
static int test_passed = 0;
static int test_failed = 0;

#define TEST_INIT() \
    do { \
        test_count = 0; \
        test_passed = 0; \
        test_failed = 0; \
    } while(0)

#define ASSERT_TRUE(condition, message) \
    do { \
        test_count++; \
        if (condition) { \
            test_passed++; \
            printf("  ✓ PASS: %s\n", message); \
        } else { \
            test_failed++; \
            printf("  ✗ FAIL: %s (line %d)\n", message, __LINE__); \
        } \
    } while(0)

#define ASSERT_FALSE(condition, message) \
    ASSERT_TRUE(!(condition), message)

#define ASSERT_EQUAL(expected, actual, message) \
    ASSERT_TRUE((expected) == (actual), message)

#define ASSERT_NOT_EQUAL(expected, actual, message) \
    ASSERT_TRUE((expected) != (actual), message)

#define ASSERT_NULL(ptr, message) \
    ASSERT_TRUE((ptr) == NULL, message)

#define ASSERT_NOT_NULL(ptr, message) \
    ASSERT_TRUE((ptr) != NULL, message)

#define ASSERT_STR_EQUAL(expected, actual, message) \
    ASSERT_TRUE(strcmp((expected), (actual)) == 0, message)

#define ASSERT_MEM_EQUAL(expected, actual, size, message) \
    ASSERT_TRUE(memcmp((expected), (actual), (size)) == 0, message)

#define RUN_TEST(test_func) \
    do { \
        printf("\nRunning %s...\n", #test_func); \
        test_func(); \
    } while(0)

#define TEST_SUMMARY() \
    do { \
        printf("\n========================================\n"); \
        printf("Test Summary:\n"); \
        printf("  Total:  %d\n", test_count); \
        printf("  Passed: %d\n", test_passed); \
        printf("  Failed: %d\n", test_failed); \
        printf("========================================\n"); \
        if (test_failed > 0) { \
            printf("RESULT: FAILED\n"); \
            return 1; \
        } else { \
            printf("RESULT: ALL TESTS PASSED\n"); \
            return 0; \
        } \
    } while(0)

#endif /* TEST_FRAMEWORK_H */
