# Makefile for alib-c tests

CC = gcc
CFLAGS = -Wall -Wextra -g -I../includes -pthread
LDFLAGS = -L.. -lalibc -lz -lpthread

# Test executables
TESTS = test_alib_string \
        test_ArrayList \
        test_DList \
        test_String \
        test_BinaryBuffer \
        test_MemPool \
        test_alib_time

# Object files
OBJS = $(TESTS:=.o)

.PHONY: all clean run

all: $(TESTS)

# Build each test executable
test_alib_string: test_alib_string.o
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

test_ArrayList: test_ArrayList.o
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

test_DList: test_DList.o
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

test_String: test_String.o
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

test_BinaryBuffer: test_BinaryBuffer.o
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

test_MemPool: test_MemPool.o
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

test_alib_time: test_alib_time.o
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# Pattern rule for object files
%.o: %.c test_framework.h
	$(CC) $(CFLAGS) -c $<

# Run all tests
run: all
	@echo "========================================"
	@echo "Running All Tests"
	@echo "========================================"
	@failed=0; \
	for test in $(TESTS); do \
		echo ""; \
		echo "----------------------------------------"; \
		echo "Running $$test..."; \
		echo "----------------------------------------"; \
		if ./$$test; then \
			echo "✓ $$test PASSED"; \
		else \
			echo "✗ $$test FAILED"; \
			failed=$$((failed + 1)); \
		fi; \
	done; \
	echo ""; \
	echo "========================================"; \
	echo "Test Run Complete"; \
	echo "========================================"; \
	if [ $$failed -eq 0 ]; then \
		echo "✓ All tests passed!"; \
		exit 0; \
	else \
		echo "✗ $$failed test(s) failed"; \
		exit 1; \
	fi

clean:
	rm -f $(TESTS) $(OBJS)
