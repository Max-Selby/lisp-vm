#include "vm.h"
#include "vmstring.h"

#include <stdio.h>
#include <stdbool.h>

// This is all testing, should be removed later
int main(void) {
    VM *vm = vm_create();
    vm->code = (Instruction[]){
        {OP_PUSH, {.as.number = 42}},
        {OP_PUSH, {.as.number = 3}},
        {OP_PRINT, {0}},
        {OP_ADD, {0}},
        {OP_PRINT, {0}},
        {OP_HALT, {0}}
    };
    vm_execute(vm);

    String s;
    printf("\n");
    string_init_from(&s, "hello\n");
    printf(s.data);
    if (!string_substr(&s, 0, 2)) {
        printf("Failure!\n");
    }
    printf(s.data);
    string_free(&s);
}

