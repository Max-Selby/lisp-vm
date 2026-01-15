#include "testutil.h"

#include <stdbool.h>
#include <stdio.h>

int test_assert(bool passed, const char *tag, const char *testPurpose) {
    if (passed) {
        return 0;
    }

    printf("- %s: TEST FAILED: %s\n", tag, testPurpose);
    return 1;
}
