#include <stdio.h>

#include "test_vmstring.h"
#include "test_vm.h"
#include "test_lexer.h"

int main() {
    int failed = 0;
    
    failed += runVMStringTests();
    failed += runVMTests();
    failed += runLexerTests();

    if (failed == 0) {
        printf("No asserts failed; all tests passed.\n");
    }
    else {
        printf("==========================\n");
        printf("%d total tests failed.\n", failed);
    }
}
