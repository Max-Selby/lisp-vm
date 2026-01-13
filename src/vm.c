#include "vm.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

#define EPSILON (1e-12)

VM* vm_create() {
    VM *vm = malloc(sizeof(VM));
    vm->stack_size = VM_STACK_SIZE;
    vm->stack = malloc(sizeof(Value) * vm->stack_size);
    vm->sp = 0;
    vm->pc = 0;
    
    return vm;
}

void vm_free(VM *vm) {
    free(vm->stack);
    free(vm);
}

void stack_push_value(VM *vm, Value value) {
    vm->stack[vm->sp] = value;
    vm->sp++;
}

void stack_push_number(VM *vm, double num) {
    Value val;
    val.type = VAL_NUMBER;
    val.as.number = num;
    stack_push_value(vm, val);
}

void stack_push_string(VM *vm, char *st) {
    Value val;
    val.type = VAL_STRING;
    val.as.string = st;
    stack_push_value(vm, val);
}

void stack_push_bool(VM *vm, bool b) {
    Value val;
    val.type = VAL_BOOL;
    val.as.boolean = b;
    stack_push_value(vm, val);
}

Value stack_pop(VM *vm) {
    vm->sp--;
    return vm->stack[vm->sp];
}

Instruction code_get_next(VM *vm) {
    Instruction insn = vm->code[vm->pc];
    vm->pc++;
    return insn;
}

void runtime_error(char *msg) {
    printf("Runtime error: %s", msg);
    exit(1);
}

void vm_execute(VM *vm) {
    while (true) {
        // Get the current instruction and increment the PC
        Instruction instruction = code_get_next(vm);

        switch (instruction.opCode) {
            case OP_PUSH: {
                stack_push_value(vm, instruction.operand);
                break;
            }
            case OP_ADD: {
                Value a = stack_pop(vm);
                Value b = stack_pop(vm);

                if (a.type != VAL_NUMBER || b.type != VAL_NUMBER) {
                    runtime_error("Cannot perform arithmetic on non-number!");
                }

                stack_push_number(a.as.number + b.as.number);
                break;
            }
            case OP_SUB: {
                Value a = stack_pop(vm);
                Value b = stack_pop(vm);

                if (a.type != VAL_NUMBER || b.type != VAL_NUMBER) {
                    runtime_error("Cannot perform arithmetic on non-number!");
                }

                stack_push_number(vm, b.as.number - a.as.number); // second - first
                break;
            }
            case OP_MUL: {
                Value a = stack_pop(vm);
                Value b = stack_pop(vm);

                if (a.type != VAL_NUMBER || b.type != VAL_NUMBER) {
                    runtime_error("Cannot perform arithmetic on non-number!");
                }

                stack_push_number(vm, a.as.number * b.as.number);
                break;
            }
            case OP_DIV: {
                Value a = stack_pop(vm);
                Value b = stack_pop(vm);

                if (a.type != VAL_NUMBER || b.type != VAL_NUMBER) {
                    runtime_error("Cannot perform arithmetic on non-number!");
                }

                if (fabs(a.as.number) < EPSILON) {
                    runtime_error("Division by 0!");
                }

                stack_push_number(vm, b.as.number / a.as.number); // second / first
            }
            case OP_LOGIC_AND: {
                Value a = stack_pop(vm);
                Value b = stack_pop(vm);

                if (a.type != VAL_BOOL || b.type != VAL_BOOL) {
                    runtime_error("Cannot perform boolean algebra on non-boolean!");
                }

                stack_push_number(vm, a.as.boolean && b.as.boolean);
                break;
            }
            case OP_LOGIC_OR: {
                Value a = stack_pop(vm);
                Value b = stack_pop(vm);

                if (a.type != VAL_BOOL || b.type != VAL_BOOL) {
                    runtime_error("Cannot perform boolean algebra on non-boolean!");
                }

                stack_push_number(vm, a.as.boolean || b.as.boolean);
                break;
            }
            case OP_LOGIC_NOT: {
                Value a = stack_pop(vm);

                if (a.type != VAL_BOOL) {
                    runtime_error("Cannot perform boolean algebra on non-boolean!");
                }

                stack_push_number(vm, !a.as.boolean);
                break;
            }
            case OP_PRINT: {
                Value val = vm->stack[vm->sp - 1];
                switch (val.type) {
                    case VAL_NUMBER:
                        printf("%d", val.as.number);
                        break;
                    case VAL_BOOL:
                        printf(val.as.bool == true ? "true" : "false");
                        break;
                    case VAL_STRING:
                        printf("%s", val.as.string);
                        break;
                }
                
                break;
            }
            case OP_HALT: {
                return;
            }
        }
    }
}

