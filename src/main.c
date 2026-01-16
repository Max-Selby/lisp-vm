#include "vm.h"
#include "vmstring.h"

#include <stdio.h>
#include <stdbool.h>

// This is all testing, should be removed later
int main(void) {
    String s1, s2;
    string_init_from(&s2, "hello");
    string_init_from(&s1, "world\n");

    VM *vm = vm_create();
    vm->code = (Instruction[]){
        {OP_PUSH, {.type = VAL_STRING, .as.string = &s1}},
        {OP_PUSH, {.type = VAL_STRING, .as.string = &s2}},
        {OP_CONCATSTR, {0}},
        {OP_PRINT, {0}},
        {OP_HALT, {0}}
    };
    vm_execute(vm);

    // VM is not responsible for freeing strings, and so it never will
    string_free(&s2);
    string_free(&s1);
}

