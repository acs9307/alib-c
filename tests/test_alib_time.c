#include "test_framework.h"
#include "../includes/alib_time.h"
#include <time.h>

#ifdef __linux__

void test_timespec_init() {
    struct timespec t;

    ASSERT_EQUAL(ALIB_OK, timespec_init(&t, 5, 500000000), "timespec_init should succeed");
    ASSERT_EQUAL(5, t.tv_sec, "Seconds should be 5");
    ASSERT_EQUAL(500000000, t.tv_nsec, "Nanoseconds should be 500000000");

    // Test overflow handling
    ASSERT_EQUAL(ALIB_OK, timespec_init(&t, 0, 1500000000), "timespec_init should handle overflow");
    ASSERT_EQUAL(1, t.tv_sec, "Seconds should overflow to 1");
    ASSERT_EQUAL(500000000, t.tv_nsec, "Nanoseconds should wrap around");
}

void test_timespec_add() {
    struct timespec a, b, sum;

    timespec_init(&a, 1, 500000000);
    timespec_init(&b, 2, 300000000);

    timespec_add(&a, &b, &sum);
    ASSERT_EQUAL(3, sum.tv_sec, "Sum seconds should be 3");
    ASSERT_EQUAL(800000000, sum.tv_nsec, "Sum nanoseconds should be 800000000");

    // Test with overflow
    timespec_init(&a, 1, 800000000);
    timespec_init(&b, 2, 700000000);
    timespec_add(&a, &b, &sum);
    ASSERT_EQUAL(4, sum.tv_sec, "Sum should handle overflow in seconds");
    ASSERT_EQUAL(500000000, sum.tv_nsec, "Sum nanoseconds should wrap");
}

void test_timespec_subtract() {
    struct timespec a, b, diff;

    timespec_init(&a, 5, 300000000);
    timespec_init(&b, 2, 100000000);

    timespec_subtract(&a, &b, &diff);
    ASSERT_EQUAL(3, diff.tv_sec, "Difference seconds should be 3");
    ASSERT_EQUAL(200000000, diff.tv_nsec, "Difference nanoseconds should be 200000000");

    // Test with borrow
    timespec_init(&a, 5, 100000000);
    timespec_init(&b, 2, 300000000);
    timespec_subtract(&a, &b, &diff);
    ASSERT_EQUAL(2, diff.tv_sec, "Difference should handle borrow");
    ASSERT_EQUAL(800000000, diff.tv_nsec, "Difference nanoseconds should wrap");
}

void test_timespec_cmp() {
    struct timespec a, b;

    timespec_init(&a, 5, 500000000);
    timespec_init(&b, 3, 500000000);

    ASSERT_EQUAL(1, timespec_cmp(a, b), "a should be greater than b");
    ASSERT_EQUAL(-1, timespec_cmp(b, a), "b should be less than a");

    timespec_init(&b, 5, 500000000);
    ASSERT_EQUAL(0, timespec_cmp(a, b), "a and b should be equal");

    timespec_init(&a, 5, 600000000);
    timespec_init(&b, 5, 500000000);
    ASSERT_EQUAL(1, timespec_cmp(a, b), "a should be greater (by nanoseconds)");
}

void test_timespec_cmp_fast() {
    struct timespec a, b;

    a.tv_sec = 5;
    a.tv_nsec = 500000000;
    b.tv_sec = 3;
    b.tv_nsec = 500000000;

    ASSERT_EQUAL(1, timespec_cmp_fast(&a, &b), "a should be greater than b");
    ASSERT_EQUAL(-1, timespec_cmp_fast(&b, &a), "b should be less than a");

    b.tv_sec = 5;
    b.tv_nsec = 500000000;
    ASSERT_EQUAL(0, timespec_cmp_fast(&a, &b), "a and b should be equal");
}

void test_timespec_fix_values() {
    struct timespec t;

    // Test overflow
    t.tv_sec = 1;
    t.tv_nsec = 1500000000;
    timespec_fix_values_fast(&t);
    ASSERT_EQUAL(2, t.tv_sec, "Seconds should be incremented");
    ASSERT_EQUAL(500000000, t.tv_nsec, "Nanoseconds should wrap");

    // Test negative nanoseconds
    t.tv_sec = 5;
    t.tv_nsec = -500000000;
    timespec_fix_values_fast(&t);
    ASSERT_EQUAL(4, t.tv_sec, "Seconds should be decremented");
    ASSERT_EQUAL(500000000, t.tv_nsec, "Nanoseconds should be positive");
}

void test_time_conversion_macros() {
    // Test seconds to other units
    ASSERT_EQUAL(1000, SECONDS_TO_MILLIS(1), "1 second = 1000 milliseconds");
    ASSERT_EQUAL(1000000, SECONDS_TO_MICROS(1), "1 second = 1000000 microseconds");
    ASSERT_EQUAL(1000000000, SECONDS_TO_NANOS(1), "1 second = 1000000000 nanoseconds");

    // Test milliseconds conversions
    ASSERT_EQUAL(0, MILLIS_TO_SECONDS(500), "500 ms = 0 seconds (integer division)");
    ASSERT_EQUAL(1, MILLIS_TO_SECONDS(1000), "1000 ms = 1 second");
    ASSERT_EQUAL(1000, MILLIS_TO_MICROS(1), "1 ms = 1000 microseconds");

    // Test microseconds conversions
    ASSERT_EQUAL(0, MICROS_TO_SECONDS(500000), "500000 us = 0 seconds");
    ASSERT_EQUAL(500, MICROS_TO_MILLIS(500000), "500000 us = 500 ms");
    ASSERT_EQUAL(1000, MICROS_TO_NANOS(1), "1 us = 1000 nanoseconds");

    // Test nanoseconds conversions
    ASSERT_EQUAL(1, NANOS_TO_MICROS(1000), "1000 ns = 1 microsecond");
    ASSERT_EQUAL(1, NANOS_TO_MILLIS(1000000), "1000000 ns = 1 millisecond");
}

#endif

int main() {
    TEST_INIT();

    printf("========================================\n");
    printf("Testing alib_time Functions\n");
    printf("========================================\n");

#ifdef __linux__
    RUN_TEST(test_timespec_init);
    RUN_TEST(test_timespec_add);
    RUN_TEST(test_timespec_subtract);
    RUN_TEST(test_timespec_cmp);
    RUN_TEST(test_timespec_cmp_fast);
    RUN_TEST(test_timespec_fix_values);
    RUN_TEST(test_time_conversion_macros);
#else
    printf("Skipping timespec tests (not on Linux)\n");
#endif

    TEST_SUMMARY();
}
