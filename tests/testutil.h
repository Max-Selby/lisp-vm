#ifndef TEST_UTIL_H
#define TEST_UTIL_H

#include <stdbool.h>

/**
 * Helper function to print if a test fails.
 * @param passed Should be true if the test succeeded, false otherwise
 * @param tag Some short tag describing the test module
 * @param testPurpose A short description of what this test is doing
 * @returns 1 on failure, 0 on success (aka "number of failed tests")
 */
int test_assert(bool passed, const char *tag, const char *testPurpose);

#endif // TEST_UTIL_H
