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

void stack_push(VM *vm, int value) {
    vm->stack[vm->sp] = value;
    vm->sp++;
}

int stack_pop(VM *vm) {
    vm->sp--;
    return vm->stack[vm->sp];
}

unsigned char code_get_next(VM *vm) {
    unsigned char value = vm->code[vm->pc];
    vm->pc++;
    return value;
}

void vm_execute(VM *vm) {
    while (true) {
        // Get the current instruction and increment the PC
        OpCode instruction = (OpCode)code_get_next(vm);

        switch (instruction) {
            case OP_PUSH: {
                int value = (int)code_get_next(vm);
                stack_push(vm, value);
                break;
            }
            case OP_ADD: {
                int a = stack_pop(vm);
                int b = stack_pop(vm);
                stack_push(vm, a + b);
                break;
            }
            case OP_SUB: {
                int a = stack_pop(vm);
                int b = stack_pop(vm);
                stack_push(vm, b - a); // second - first
                break;
            }
            case OP_MUL: {
                int a = stack_pop(vm);
                int b = stack_pop(vm);
                stack_push(vm, a * b);
                break;
            }
            case OP_PRINT_INT: {
                printf("%d\n", vm->stack[vm->sp - 1]);
                break;
            }
            case OP_HALT: {
                return;
            }
        }
    }
}

