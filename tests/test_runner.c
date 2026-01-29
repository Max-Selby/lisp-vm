#include <stdio.h>

#include "test_vmstring.h"
#include "test_vm.h"
#include "test_lexer.h"
#include "test_parser.h"

int main() {
    int failed = 0;
    
    failed += run_vm_string_tests();
    failed += run_vm_tests();
    failed += run_lexer_tests();
    failed += run_parser_tests();

    if (failed == 0) {
        printf("No asserts failed; all tests passed.\n");
    }
    else {
        printf("==========================\n");
        printf("%d total tests failed.\n", failed);
    }
}
