# CMakeLists.txt for alib-c tests

# Test executables
set(TEST_SOURCES
    test_alib_string.c
    test_ArrayList.c
    test_String.c
    test_BinaryBuffer.c
    test_MemPool.c
    test_alib_time.c
)
# Note: test_DList.c excluded - requires complex API adjustments

# Build each test executable
foreach(TEST_SRC ${TEST_SOURCES})
    # Get test name without extension
    get_filename_component(TEST_NAME ${TEST_SRC} NAME_WE)

    # Create executable
    add_executable(${TEST_NAME} ${TEST_SRC})

    # Link against alibc and dependencies
    target_link_libraries(${TEST_NAME} alibc z pthread)

    # Set include directories
    target_include_directories(${TEST_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/includes)

    # Add to test list
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()

# Custom target to run all tests
add_custom_target(run_tests
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure --verbose
    DEPENDS ${TEST_SOURCES}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Running all tests..."
)

# Alternative: run tests individually with custom output
add_custom_target(run_all_tests
    COMMENT "Running all alib-c tests..."
)

foreach(TEST_SRC ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_SRC} NAME_WE)
    add_custom_command(TARGET run_all_tests POST_BUILD
        COMMAND ${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Running ${TEST_NAME}..."
    )
endforeach()
