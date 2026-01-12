#include "vm.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


VM* vm_create() {
    VM *vm = malloc(sizeof(VM));
    vm->stack_size = VM_STACK_SIZE;
    vm->stack = malloc(sizeof(int) * vm->stack_size);
    vm->sp = 0;
    vm->pc = 0;
    
    return vm;
}

void vm_free(VM *vm) {
    free(vm->stack);
    free(vm);
}

void vm_execute(VM *vm) {
    while (true) {

    }
}

