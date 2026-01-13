#include "vm.h"

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
}

