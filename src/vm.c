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

void stack_push_integer(VM *vm, int num) {
    Value val;
    val.type = VAL_INTEGER;
    val.as.integer = num;
    stack_push_value(vm, val);
}

void stack_push_float(VM *vm, double num) {
    Value val;
    val.type = VAL_FLOAT;
    val.as.floating = num;
    stack_push_value(vm, val);
}

void stack_push_string(VM *vm, String *st) {
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

                if (
                    (a.type != VAL_INTEGER && a.type != VAL_FLOAT) || 
                    (b.type != VAL_INTEGER && b.type != VAL_FLOAT)
                ) {
                    runtime_error("Cannot perform arithmetic on non-number!");
                }
                
                if (a.type == VAL_INTEGER && b.type == VAL_INTEGER) {
                    stack_push_integer(vm, a.as.integer + b.as.integer);
                }
                else {
                    float anum = (a.type == VAL_INTEGER) ? (float)a.as.integer : a.as.floating;
                    float bnum = (b.type == VAL_INTEGER) ? (float)b.as.integer : b.as.floating;

                    stack_push_float(vm, anum + bnum);
                }

                break;
            }
            case OP_SUB: {
                Value a = stack_pop(vm);
                Value b = stack_pop(vm);

                if (
                    (a.type != VAL_INTEGER && a.type != VAL_FLOAT) || 
                    (b.type != VAL_INTEGER && b.type != VAL_FLOAT)
                ) {
                    runtime_error("Cannot perform arithmetic on non-number!");
                }

                // second - first
                if (a.type == VAL_INTEGER && b.type == VAL_INTEGER) {
                    stack_push_integer(vm, b.as.integer - a.as.integer); 
                }
                else {
                    float anum = (a.type == VAL_INTEGER) ? (float)a.as.integer : a.as.floating;
                    float bnum = (b.type == VAL_INTEGER) ? (float)b.as.integer : b.as.floating;

                    stack_push_float(vm, bnum - anum);
                }
                
                break;
            }
            case OP_MUL: {
                Value a = stack_pop(vm);
                Value b = stack_pop(vm);

                if (
                    (a.type != VAL_INTEGER && a.type != VAL_FLOAT) || 
                    (b.type != VAL_INTEGER && b.type != VAL_FLOAT)
                ) {
                    runtime_error("Cannot perform arithmetic on non-number!");
                }

                if (a.type == VAL_INTEGER && b.type == VAL_INTEGER) {
                    stack_push_integer(vm, a.as.integer * b.as.integer);
                }
                else {
                    float anum = (a.type == VAL_INTEGER) ? (float)a.as.integer : a.as.floating;
                    float bnum = (b.type == VAL_INTEGER) ? (float)b.as.integer : b.as.floating;

                    stack_push_float(vm, anum * bnum);
                }
                break;
            }
            case OP_DIV: {
                Value a = stack_pop(vm);
                Value b = stack_pop(vm);

                if (
                    (a.type != VAL_INTEGER && a.type != VAL_FLOAT) || 
                    (b.type != VAL_INTEGER && b.type != VAL_FLOAT)
                ) {
                    runtime_error("Cannot perform arithmetic on non-number!");
                }

                float anum = (a.type == VAL_INTEGER) ? (float)a.as.integer : a.as.floating;
                float bnum = (b.type == VAL_INTEGER) ? (float)b.as.integer : b.as.floating;

                if (fabs(bnum) < EPSILON) {
                    runtime_error("Division by 0!");
                }

                // second / first
                stack_push_float(vm, bnum / anum);
                
                break;
            }
            case OP_LOGIC_AND: {
                Value a = stack_pop(vm);
                Value b = stack_pop(vm);

                if (a.type != VAL_BOOL || b.type != VAL_BOOL) {
                    runtime_error("Cannot perform boolean algebra on non-boolean!");
                }

                stack_push_bool(vm, a.as.boolean && b.as.boolean);
                break;
            }
            case OP_LOGIC_OR: {
                Value a = stack_pop(vm);
                Value b = stack_pop(vm);

                if (a.type != VAL_BOOL || b.type != VAL_BOOL) {
                    runtime_error("Cannot perform boolean algebra on non-boolean!");
                }

                stack_push_bool(vm, a.as.boolean || b.as.boolean);
                break;
            }
            case OP_LOGIC_NOT: {
                Value a = stack_pop(vm);

                if (a.type != VAL_BOOL) {
                    runtime_error("Cannot perform boolean algebra on non-boolean!");
                }

                stack_push_bool(vm, !a.as.boolean);
                break;
            }
            case OP_PRINT: {
                Value val = vm->stack[vm->sp - 1];
                switch (val.type) {
                    case VAL_INTEGER:
                        printf("%d", val.as.integer);
                        break;
                    case VAL_FLOAT:
                        printf("%f", val.as.floating);
                        break;
                    case VAL_BOOL:
                        printf(val.as.boolean == true ? "true" : "false");
                        break;
                    case VAL_STRING:
                        printf("%s", val.as.string->data);
                        break;
                }
                
                break;
            }
            case OP_CONCATSTR: {
                Value a = stack_pop(vm);
                Value b = stack_pop(vm);

                if (a.type != VAL_STRING || b.type != VAL_STRING) {
                    runtime_error("Cannot concatenate non-strings!");
                }
                
                bool success = string_append(a.as.string, b.as.string->data);

                if (!success) {
                    runtime_error("String append failed!");
                }

                stack_push_string(vm, a.as.string);

                break;
            }
            case OP_SUBSTR: {
                Value s = stack_pop(vm);
                Value start = stack_pop(vm);
                Value length = stack_pop(vm);

                if (s.type != VAL_STRING) {
                    runtime_error("Cannot take substring of non-string!");
                }

                if (start.type != VAL_INTEGER || length.type != VAL_INTEGER) {
                    runtime_error("Start and length of substring must be integers!");
                }

                if (start.as.integer < 0 || length.as.integer < 0) {
                    runtime_error("Start and length of substring may not be negative!");
                }

                bool success = string_substr(s.as.string, (size_t)start.as.integer, (size_t)length.as.integer);

                if (!success) {
                    runtime_error("String substring failed!");
                }

                stack_push_string(vm, s.as.string);

                break;
            }
            case OP_DISCARD: {
                stack_pop(vm);
                break;
            }
            case OP_DUP: {
                Value a = stack_pop(vm);
                stack_push_value(vm, a);
                stack_push_value(vm, a);
                break;
            }
            case OP_HALT: {
                return;
            }
        }
    }
}

