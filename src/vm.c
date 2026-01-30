#include "vm.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <limits.h>

#define EPSILON (1e-12)

VM* vm_create() {
    VM *vm = malloc(sizeof(VM));
    vm->stack_cap = 256;
    vm->stack = malloc(sizeof(Value) * vm->stack_cap);
    vm->sp = 0;
    vm->pc = 0;
    vm->debug = false;
    vm->strings_cap = 8;
    vm->strings_count = 0;
    vm->strings = malloc(sizeof(String*) * vm->strings_cap);
    
    return vm;
}

void vm_free(VM *vm) {
    if (vm->debug) {
        printf("Freeing VM\n");
    }
    // Cleanup strings
    for (size_t i = 0; i < vm->strings_count; i++) {
        printf("String i: %d\n", (int)i);
        string_free(vm->strings[i]);
    }
    free(vm->strings);

    free(vm->stack);
    free(vm);
}

void runtime_error(char *msg) {
    printf("Runtime error: %s\n", msg);
    exit(1);
}

void stack_push_value(VM *vm, Value value) {
    if (vm->sp >= (int)vm->stack_cap) {
        size_t new_cap = vm->stack_cap * 2;
        Value *tmp = realloc(vm->stack, new_cap * sizeof *vm->stack);
        if (!tmp) {
            runtime_error("Unable to allocate space for stack growth");
        }
        vm->stack = tmp;
        vm->stack_cap = new_cap;
    }

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
    if (vm->sp <= 0) {
        runtime_error("Stack underflow!");
    }
    vm->sp--;
    return vm->stack[vm->sp];
}

Instruction code_get_next(VM *vm) {
    Instruction insn = vm->code[vm->pc];
    vm->pc++;
    return insn;
}

String *vm_string_new(VM *vm) {
    if (vm->strings_count == vm->strings_cap) {
        size_t new_cap = vm->strings_cap * 2;
        String **tmp = realloc(vm->strings, new_cap * sizeof *vm->strings);
        if (!tmp) {
            runtime_error("Unable to allocate space for new strings");
        }
        vm->strings = tmp;
        vm->strings_cap = new_cap;
    }

    String *s = string_create();
    if (!s) {
        runtime_error("Unable to create new string");
    }
    vm->strings[vm->strings_count] = s;
    if (vm->debug) {
        printf("Allocated string at index %d\n", (int)vm->strings_count);
    }
    vm->strings_count++;

    return s;
}

void vm_execute(VM *vm) {
    while (true) {
        if (vm->debug) {
            printf("=> PC: %d\n", vm->pc);
        }
        // Get the current instruction and increment the PC
        Instruction instruction = code_get_next(vm);

        if (vm->debug) {
            printf("insn num: %d\n", instruction.opCode);
        }

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
                    double anum = (a.type == VAL_INTEGER) ? (double)a.as.integer : a.as.floating;
                    double bnum = (b.type == VAL_INTEGER) ? (double)b.as.integer : b.as.floating;

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
                    double anum = (a.type == VAL_INTEGER) ? (double)a.as.integer : a.as.floating;
                    double bnum = (b.type == VAL_INTEGER) ? (double)b.as.integer : b.as.floating;

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
                    double anum = (a.type == VAL_INTEGER) ? (double)a.as.integer : a.as.floating;
                    double bnum = (b.type == VAL_INTEGER) ? (double)b.as.integer : b.as.floating;

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

                double anum = (a.type == VAL_INTEGER) ? (double)a.as.integer : a.as.floating;
                double bnum = (b.type == VAL_INTEGER) ? (double)b.as.integer : b.as.floating;

                if (fabs(bnum) < EPSILON) {
                    runtime_error("Division by 0!");
                }

                // second / first
                stack_push_float(vm, bnum / anum);
                
                break;
            }
            case OP_MOD: {
                Value a = stack_pop(vm);
                Value b = stack_pop(vm);

                if (a.type != VAL_INTEGER || b.type != VAL_INTEGER) {
                    runtime_error("Cannot perform modulo on non-integer!");
                }

                if (a.as.integer == 0) {
                    runtime_error("Modulo by 0!");
                }

                // second % first
                stack_push_integer(vm, b.as.integer % a.as.integer);
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
            case OP_PRINTLN: {
                Value val = vm->stack[vm->sp - 1];
                switch (val.type) {
                    case VAL_INTEGER:
                        printf("%d\n", val.as.integer);
                        break;
                    case VAL_FLOAT:
                        printf("%f\n", val.as.floating);
                        break;
                    case VAL_BOOL:
                        printf(val.as.boolean == true ? "true\n" : "false\n");
                        break;
                    case VAL_STRING:
                        printf("%s\n", val.as.string->data);
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
                
                String *new = string_copy(a.as.string);
                bool success = string_append(new, b.as.string->data);

                if (!success) {
                    runtime_error("String append failed!");
                }

                stack_push_string(vm, new);

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

                String *new = string_copy(s.as.string);
                bool success = string_substr(new, (size_t)start.as.integer, (size_t)length.as.integer);

                if (!success) {
                    runtime_error("String substring failed!");
                }

                stack_push_string(vm, new);

                break;
            }
            case OP_DISCARD: {
                stack_pop(vm);
                break;
            }
            case OP_DUP: {
                Value a = stack_pop(vm);
                if (a.type == VAL_STRING) {
                    String *new = string_copy(a.as.string);
                    stack_push_value(vm, a);
                    stack_push_string(vm, new);
                }
                else {
                    stack_push_value(vm, a);
                    stack_push_value(vm, a);
                }
                break;
            }
            case OP_SWAP: {
                Value a = stack_pop(vm);
                Value b = stack_pop(vm);

                stack_push_value(vm, a);
                stack_push_value(vm, b);
                break;
            }
            case OP_EQ: {
                Value a = stack_pop(vm);
                Value b = stack_pop(vm);

                if (
                    (a.type != VAL_INTEGER && a.type != VAL_FLOAT) ||
                    (b.type != VAL_INTEGER && b.type != VAL_FLOAT)
                ) {
                    runtime_error("Cannot compare equality of non-numbers!");
                }

                double anum = (a.type == VAL_INTEGER) ? (double)a.as.integer : a.as.floating;
                double bnum = (b.type == VAL_INTEGER) ? (double)b.as.integer : b.as.floating;

                stack_push_bool(vm, anum == bnum);
                break;
            }
            case OP_NEQ: {
                Value a = stack_pop(vm);
                Value b = stack_pop(vm);

                if (
                    (a.type != VAL_INTEGER && a.type != VAL_FLOAT) ||
                    (b.type != VAL_INTEGER && b.type != VAL_FLOAT)
                ) {
                    runtime_error("Cannot compare equality of non-numbers!");
                }

                double anum = (a.type == VAL_INTEGER) ? (double)a.as.integer : a.as.floating;
                double bnum = (b.type == VAL_INTEGER) ? (double)b.as.integer : b.as.floating;

                stack_push_bool(vm, anum != bnum);
                break;
            }
            case OP_LT: {
                Value a = stack_pop(vm);
                Value b = stack_pop(vm);

                if (
                    (a.type != VAL_INTEGER && a.type != VAL_FLOAT) ||
                    (b.type != VAL_INTEGER && b.type != VAL_FLOAT)
                ) {
                    runtime_error("Cannot compare equality of non-numbers!");
                }

                double anum = (a.type == VAL_INTEGER) ? (double)a.as.integer : a.as.floating;
                double bnum = (b.type == VAL_INTEGER) ? (double)b.as.integer : b.as.floating;

                stack_push_bool(vm, anum < bnum);
                break;
            }
            case OP_LTE: {
                Value a = stack_pop(vm);
                Value b = stack_pop(vm);

                if (
                    (a.type != VAL_INTEGER && a.type != VAL_FLOAT) ||
                    (b.type != VAL_INTEGER && b.type != VAL_FLOAT)
                ) {
                    runtime_error("Cannot compare equality of non-numbers!");
                }

                double anum = (a.type == VAL_INTEGER) ? (double)a.as.integer : a.as.floating;
                double bnum = (b.type == VAL_INTEGER) ? (double)b.as.integer : b.as.floating;

                stack_push_bool(vm, anum <= bnum);
                break;
            }
            case OP_GT: {
                Value a = stack_pop(vm);
                Value b = stack_pop(vm);

                if (
                    (a.type != VAL_INTEGER && a.type != VAL_FLOAT) ||
                    (b.type != VAL_INTEGER && b.type != VAL_FLOAT)
                ) {
                    runtime_error("Cannot compare equality of non-numbers!");
                }

                double anum = (a.type == VAL_INTEGER) ? (double)a.as.integer : a.as.floating;
                double bnum = (b.type == VAL_INTEGER) ? (double)b.as.integer : b.as.floating;

                stack_push_bool(vm, anum > bnum);
                break;
            }
            case OP_GTE: {
                Value a = stack_pop(vm);
                Value b = stack_pop(vm);

                if (
                    (a.type != VAL_INTEGER && a.type != VAL_FLOAT) ||
                    (b.type != VAL_INTEGER && b.type != VAL_FLOAT)
                ) {
                    runtime_error("Cannot compare equality of non-numbers!");
                }

                double anum = (a.type == VAL_INTEGER) ? (double)a.as.integer : a.as.floating;
                double bnum = (b.type == VAL_INTEGER) ? (double)b.as.integer : b.as.floating;

                stack_push_bool(vm, anum >= bnum);
                break;
            }
            case OP_STR_EQ: {
                Value a = stack_pop(vm);
                Value b = stack_pop(vm);

                if (a.type != VAL_STRING || b.type != VAL_STRING) {
                    runtime_error("Tried to check string equivalence of non-strings!");
                }

                bool equiv;
                bool success = string_equal(a.as.string, b.as.string, &equiv);

                if (!success) {
                    runtime_error("String equal failed! Strings were probably not initialized.");
                }

                stack_push_bool(vm, equiv);
                break;
            }
            case OP_STRLEN: {
                Value a = stack_pop(vm);

                if (a.type != VAL_STRING) {
                    runtime_error("Tried to get string length of non-string!");
                }

                int len;
                bool success = string_length(a.as.string, &len);

                if (!success) {
                    runtime_error("String length failed! String was probably not initialized.");
                }

                stack_push_integer(vm, len);
                break;
            }
            case OP_JMP: {
                Value a = stack_pop(vm);

                if (a.type != VAL_INTEGER) {
                    runtime_error("Cannot jump to non-integer address!");
                }
                if (a.as.integer < 0) {
                    runtime_error("Cannot jump to negative address!");
                }
                
                // Jump there. Don't have to worry about PC increasing, since that happens at the
                // beginning of the loop AFTER grabbing the instruction
                vm->pc = a.as.integer;
                break;
            }
            case OP_JMP_IF: {
                Value a = stack_pop(vm);
                Value condition = stack_pop(vm);

                if (a.type != VAL_INTEGER) {
                    runtime_error("Cannot jump to non-integer address!");
                }
                if (a.as.integer < 0) {
                    runtime_error("Cannot jump to negative address!");
                }
                if (condition.type != VAL_BOOL) {
                    runtime_error("Conditional jump failed: wrong condition type (should be boolean)");
                }
                
                if (condition.as.boolean) {
                    vm->pc = a.as.integer;
                }
                break;
            }
            case OP_INT2FLOAT: {
                Value a = stack_pop(vm);

                if (a.type == VAL_FLOAT) {
                    stack_push_float(vm, a.as.floating);
                }
                else if (a.type == VAL_INTEGER) {
                    stack_push_float(vm, (double)a.as.integer);
                }
                else {
                    runtime_error("Cannot convert non-number to float!");
                }
                
                break;
            }
            case OP_FLOAT2INT: {
                Value a = stack_pop(vm);

                if (a.type == VAL_INTEGER) {
                    stack_push_integer(vm, a.as.integer);
                }
                else if (a.type == VAL_FLOAT) {
                    if ((INT_MIN <= a.as.floating) && (a.as.floating <= INT_MAX)) {
                        stack_push_integer(vm, (int)a.as.floating);
                    }
                    else {
                        runtime_error("This float is too big for integer conversion!");
                    }
                }
                else {
                    runtime_error("Cannot convert non-number to integer!");
                }
                break;
            }
            case OP_HALT: {
                return;
            }
        }
    }
}
