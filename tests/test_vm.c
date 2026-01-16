#include "test_vm.h"

#include "testutil.h"
#include "vm.h"

#include <stdio.h>

const char *TAG_VM = "TEST_VM";

int runVMTests() {
    int failed = 0;
    failed += 0; // TODO: Add tests

    if (failed > 0) {
        printf("%s: Tests failed: %d\n", TAG_VM, failed);
    }
    return failed;
}
