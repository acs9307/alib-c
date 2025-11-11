# alib-c Test Suite

Comprehensive test coverage for the alib-c library.

## Test Modules

| Module | Tests | Coverage |
|--------|-------|----------|
| test_alib_string | 56 | String manipulation utilities |
| test_ArrayList | 24 | Dynamic array list operations |
| test_String | 29 | String class functionality |
| test_BinaryBuffer | 25 | Binary buffer management |
| test_MemPool | 17 | Memory pool allocation |
| test_alib_time | 36 | Time utilities and conversions |
| **TOTAL** | **187** | **All core modules** |

## Building and Running Tests

### Using CMake (Recommended)

Build the library with tests enabled (default):
```bash
mkdir build
cd build
cmake ..
make
```

Run all tests using CTest:
```bash
ctest --output-on-failure
```

Run tests with verbose output:
```bash
ctest --output-on-failure --verbose
```

Run a specific test:
```bash
./tests/test_ArrayList
```

### Build Options

Disable tests during build:
```bash
cmake -DBUILD_TESTS=OFF ..
make
```

### Alternative: Using Custom Target

Run all tests with detailed output:
```bash
make run_all_tests
```

## Test Framework

Tests use a custom lightweight framework defined in `test_framework.h` with the following assertion macros:

- `ASSERT_TRUE(condition, message)` - Assert condition is true
- `ASSERT_FALSE(condition, message)` - Assert condition is false
- `ASSERT_EQUAL(expected, actual, message)` - Assert equality
- `ASSERT_NOT_EQUAL(expected, actual, message)` - Assert inequality
- `ASSERT_NULL(ptr, message)` - Assert pointer is NULL
- `ASSERT_NOT_NULL(ptr, message)` - Assert pointer is not NULL
- `ASSERT_STR_EQUAL(expected, actual, message)` - Assert string equality
- `ASSERT_MEM_EQUAL(expected, actual, size, message)` - Assert memory equality

## Test Structure

Each test file follows this pattern:

```c
#include "test_framework.h"
#include "../includes/ModuleName.h"

void test_feature_name() {
    // Setup
    Module* obj = newModule();

    // Test
    int result = Module_do_something(obj);

    // Assert
    ASSERT_EQUAL(expected, result, "Should do something correctly");

    // Cleanup
    delModule(&obj);
}

int main() {
    TEST_INIT();

    RUN_TEST(test_feature_name);

    TEST_SUMMARY();
}
```

## Coverage Details

### test_alib_string (56 tests)
- String copying functions (strcpy_safe, strcpy_back, strncpy variants)
- Memory operations (memcpy_safe, memcpy_back)
- String allocation (strcpy_alloc, strncpy_alloc)
- String comparison (str_match, str_match_no_case)
- Case conversion (str_to_lower, str_to_upper)
- Number conversion (itoa_alloc, uitoa_alloc, ftoa_alloc)
- String searching (find_next_whitespace, find_last_char, etc.)
- Substring operations (make_substr)

### test_ArrayList (24 tests)
- Object lifecycle (creation, destruction)
- Adding and removing items
- Item retrieval by index
- Finding items by value
- List operations (clear, resize, expand)
- Capacity management

### test_String (29 tests)
- String class lifecycle
- Setting and appending operations
- Character and integer appending
- String manipulation (clear, clear_no_free)
- Length and capacity tracking
- C-string conversion

### test_BinaryBuffer (25 tests)
- Buffer lifecycle
- Appending and inserting data
- Buffer expansion and resizing
- Data removal
- Capacity management
- Raw buffer access

### test_MemPool (17 tests)
- Memory pool creation and destruction
- Block reservation and release
- Multiple concurrent reservations
- Block reuse after release
- Data integrity verification

### test_alib_time (36 tests)
- timespec initialization and manipulation
- Time arithmetic (add, subtract)
- Time comparison functions
- Value fixing and normalization
- Time unit conversions (seconds, milliseconds, microseconds, nanoseconds)

## Adding New Tests

1. Create a new test file in the `tests/` directory (e.g., `test_NewModule.c`)
2. Add it to `TEST_SOURCES` in `tests/CMakeLists.txt`
3. Rebuild: `cd build && cmake .. && make`
4. Run: `ctest` or `make run_all_tests`

## Test Results

All tests currently pass with a 100% success rate:

```
100% tests passed, 0 tests failed out of 6
```

## Future Work

- Add tests for DList (requires complex API work)
- Add tests for network modules (TcpClient, TcpServer)
- Add tests for threading modules (Timer, ThreadedTimerEvent)
- Add tests for file I/O operations
- Integrate with CI/CD pipeline
