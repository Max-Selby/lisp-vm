#include <stdio.h>

#include "test_vmstring.h"

int main() {
    int failed = 0;
    
    failed += runVMStringTests();

    if (failed == 0) {
        printf("No asserts failed; all tests passed.\n");
    }
    else {
        printf("==========================\n");
        printf("%d total tests failed.\n", failed);
    }
}
