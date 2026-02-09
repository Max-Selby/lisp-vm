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
    
    vm->globals_cap = 8;
    vm->globals = malloc(sizeof(Value) * vm->globals_cap);

    vm->allocated_lists_cap = 8;
    vm->allocated_lists_count = 0;
    vm->allocated_lists = malloc(sizeof(List*) * vm->allocated_lists_cap);

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

    // Cleanup lists
    for (size_t i = 0; i < vm->allocated_lists_count; i++) {
        free(vm->allocated_lists[i]->elements);
        free(vm->allocated_lists[i]);
    }
    free(vm->allocated_lists);
    
    free(vm->strings);
    free(vm->globals);

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

void globals_store(VM *vm, int location, Value value) {
    if (location < 0) {
        runtime_error("Global variable location out of bounds");
    }

    // Create more space if needed
    if ((size_t)location >= vm->globals_cap) {
        size_t new_cap = vm->globals_cap;
        while ((size_t)location >= new_cap) {
            new_cap *= 2;
        }
        Value *tmp = realloc(vm->globals, new_cap * sizeof *vm->globals);
        if (!tmp) {
            runtime_error("Unable to allocate space for globals growth");
        }
        vm->globals = tmp;
        vm->globals_cap = new_cap;
    }

    // Store the value
    vm->globals[location] = value;
}

Value globals_load(VM *vm, int location) {
    if (location < 0 || (size_t)location >= vm->globals_cap) {
        runtime_error("Global variable location out of bounds");
    }

    return vm->globals[location];
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

void print_list(List *list) {
    printf("[");
    for (size_t i = 0; i < list->count; i++) {
        Value val = list->elements[i];
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
                // Surround string with quotes in this case to avoid confusion
                printf("\"%s\"", val.as.string->data);
                break;
            case VAL_LIST:
                print_list(val.as.list);
                break;
        }
        if (i < list->count - 1) {
            printf(" ");
        }
    }
    printf("]");
}

void vm_register_list(VM *vm, List *list) {
    if (vm->allocated_lists_count >= vm->allocated_lists_cap) {
        vm->allocated_lists_cap *= 2;
        List **tmp = realloc(
            vm->allocated_lists,
            sizeof(List*) * vm->allocated_lists_cap
        );
        if (tmp == NULL) {
            runtime_error("Failed to reallocate allocated lists!");
        }
        vm->allocated_lists = tmp;
    }
    vm->allocated_lists[vm->allocated_lists_count] = list;
    vm->allocated_lists_count++;
}

List *list_copy(List *source) {
    if (!source) return NULL;
    List *copy = malloc(sizeof(List));
    copy->count = source->count;
    copy->capacity = source->capacity;
    copy->elements = malloc(sizeof(Value) * copy->capacity);
    for (size_t i = 0; i < copy->count; i++) {
        copy->elements[i] = source->elements[i];
    }
    return copy;
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
            case OP_LOAD_VAR: {
                // Load a value from a global variable and push it onto the stack
                if (instruction.operand.type != VAL_INTEGER) {
                    runtime_error("Variable location must be an integer!");
                }
                int location = instruction.operand.as.integer;
                Value val = globals_load(vm, location);
                stack_push_value(vm, val);
                break;
            }
            case OP_STORE_VAR: {
                // Store a value into a global variable
                if (instruction.operand.type != VAL_INTEGER) {
                    runtime_error("Variable location must be an integer!");
                }
                int location = instruction.operand.as.integer;
                Value val = stack_pop(vm);
                globals_store(vm, location, val);

                // Also push it back onto the stack as a return value
                stack_push_value(vm, val);
                break;
            }
            case OP_MAKE_LIST: {
                if (instruction.operand.type != VAL_INTEGER) {
                    runtime_error("Make list operand must be an integer!");
                }
                int count = instruction.operand.as.integer;
                
                List *list = malloc(sizeof(List));
                list->count = (size_t)count;
                list->capacity = (size_t)count;
                if (list->capacity < 8) {
                    list->capacity = 8;
                }
                list->elements = malloc(sizeof(Value) * list->capacity);

                // Pop from stack in reverse order so that the first element ends up at the front of the list
                for (size_t i = 0; i < list->count; i++) {
                    list->elements[list->count - 1 - i] = stack_pop(vm);
                }

                // Add to allocated lists for cleanup later
                vm_register_list(vm, list);

                // Push the list onto the stack
                Value val;
                val.type = VAL_LIST;
                val.as.list = list;
                stack_push_value(vm, val);

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
                Value val = stack_pop(vm);
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
                    case VAL_LIST:
                        print_list(val.as.list);
                        break;
                }
                
                // Push it back as a return value
                stack_push_value(vm, val);

                break;
            }
            case OP_PRINTLN: {
                Value val = stack_pop(vm);
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
                    case VAL_LIST:
                        print_list(val.as.list);
                        printf("\n");
                        break;
                }
                
                // Push it back as a return value
                stack_push_value(vm, val);

                break;
            }
            case OP_CONCATSTR: {
                Value a = stack_pop(vm);
                Value b = stack_pop(vm);

                if (a.type != VAL_STRING || b.type != VAL_STRING) {
                    runtime_error("Cannot concatenate non-strings!");
                }
                
                String *new = string_copy(b.as.string);
                bool success = string_append(new, a.as.string->data);

                if (!success) {
                    runtime_error("String append failed!");
                }

                stack_push_string(vm, new);

                break;
            }
            case OP_SUBSTR: {
                Value length = stack_pop(vm);
                Value start = stack_pop(vm);
                Value s = stack_pop(vm);

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

                stack_push_bool(vm, bnum < anum);
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

                stack_push_bool(vm, bnum <= anum);
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

                stack_push_bool(vm, bnum > anum);
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

                stack_push_bool(vm, bnum >= anum);
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
            case OP_JMP_IF_FALSE: {
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

                if (!condition.as.boolean) {
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
            case OP_LIST_APPEND: {
                Value source_list = stack_pop(vm);
                Value the_val = stack_pop(vm);

                if (source_list.type != VAL_LIST) {
                    runtime_error("Cannot append to non-list!");
                }

                List *new_list = list_copy(source_list.as.list);
                vm_register_list(vm, new_list);

                // Grow new list if needed
                if (new_list->count + 1 >= new_list->capacity) {
                    new_list->capacity *= 2;
                    Value *tmp = realloc(new_list->elements, sizeof(Value) * new_list->capacity);
                    if (!tmp) {
                        runtime_error("Unable to allocate space for list append!");
                    }
                    new_list->elements = tmp;
                }

                // Add value to end of new list
                new_list->elements[new_list->count] = the_val;
                new_list->count++;

                // Push the new list onto the stack
                Value val;
                val.type = VAL_LIST;
                val.as.list = new_list;
                stack_push_value(vm, val);

                break;
            }
            case OP_LIST_SUBLIST: {
                Value length_val = stack_pop(vm);
                Value start_val = stack_pop(vm);
                Value source_list = stack_pop(vm);

                if (source_list.type != VAL_LIST) {
                    runtime_error("Cannot take sublist of non-list!");
                }
                if (start_val.type != VAL_INTEGER || length_val.type != VAL_INTEGER) {
                    runtime_error("Start and length of sublist must be integers!");
                }
                if (start_val.as.integer < 0 || length_val.as.integer < 0) {
                    runtime_error("Start and length of sublist may not be negative!");
                }
                if ((size_t)start_val.as.integer >= source_list.as.list->count) {
                    runtime_error("Sublist start index out of bounds!");
                }
                if ((size_t)(start_val.as.integer + length_val.as.integer) > source_list.as.list->count) {
                    runtime_error("Sublist length goes out of bounds!");
                }

                List *new_list = list_copy(source_list.as.list);
                vm_register_list(vm, new_list);
                new_list->count = (size_t)length_val.as.integer;
                for (size_t i = 0; i < new_list->count; i++) {
                    new_list->elements[i] = source_list.as.list->elements[(size_t)start_val.as.integer + i];
                }

                // Push the new list onto the stack
                Value val;
                val.type = VAL_LIST;
                val.as.list = new_list;
                stack_push_value(vm, val);

                break;
            }
            case OP_LIST_REMOVE: {
                Value index_val = stack_pop(vm);
                Value source_list = stack_pop(vm);

                if (source_list.type != VAL_LIST) {
                    runtime_error("Cannot remove from non-list!");
                }
                if (index_val.type != VAL_INTEGER) {
                    runtime_error("Index of list element to remove must be an integer!");
                }
                if (index_val.as.integer < 0 || (size_t)index_val.as.integer >= source_list.as.list->count) {
                    runtime_error("Index of list element to remove is out of bounds!");
                }

                List *new_list = list_copy(source_list.as.list);
                vm_register_list(vm, new_list);
                size_t index = (size_t)index_val.as.integer;
                for (size_t i = 0; i < source_list.as.list->count; i++) {
                    if (i < index) {
                        new_list->elements[i] = source_list.as.list->elements[i];
                    }
                    else if (i > index) {
                        new_list->elements[i - 1] = source_list.as.list->elements[i];
                    }
                }
                new_list->count--;

                // Push the new list onto the stack
                Value val;
                val.type = VAL_LIST;
                val.as.list = new_list;
                stack_push_value(vm, val);

                break;
            }
            case OP_LIST_SET: {
                Value the_val = stack_pop(vm);
                Value index_val = stack_pop(vm);
                Value source_list = stack_pop(vm);

                if (source_list.type != VAL_LIST) {
                    runtime_error("Cannot list-set element of non-list!");
                }
                if (index_val.type != VAL_INTEGER) {
                    runtime_error("Index of list element to set must be an integer!");
                }
                if (index_val.as.integer < 0 || (size_t)index_val.as.integer >= source_list.as.list->count) {
                    runtime_error("Index of list element to set is out of bounds!");
                }

                List *new_list = list_copy(source_list.as.list);
                vm_register_list(vm, new_list);
                new_list->elements[(size_t)index_val.as.integer] = the_val;

                // Push the new list onto the stack
                Value val;
                val.type = VAL_LIST;
                val.as.list = new_list;
                stack_push_value(vm, val);

                break;
            }
            case OP_LIST_GET: {
                Value index_val = stack_pop(vm);
                Value source_list = stack_pop(vm);

                if (source_list.type != VAL_LIST) {
                    runtime_error("Cannot list-get element of non-list!");
                }
                if (index_val.type != VAL_INTEGER) {
                    runtime_error("Index of list element to get must be an integer!");
                }
                if (index_val.as.integer < 0 || (size_t)index_val.as.integer >= source_list.as.list->count) {
                    runtime_error("Index of list element to get is out of bounds!");
                }

                Value val = source_list.as.list->elements[(size_t)index_val.as.integer];
                stack_push_value(vm, val);

                break;
            }
            case OP_LIST_LEN: {
                Value source_list = stack_pop(vm);

                if (source_list.type != VAL_LIST) {
                    runtime_error("Cannot get length of non-list!");
                }

                int len = (int)source_list.as.list->count;
                stack_push_integer(vm, len);

                break;
            }
            case OP_HALT: {
                return;
            }
        }
    }
}
