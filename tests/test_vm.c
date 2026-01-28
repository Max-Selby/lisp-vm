#include "test_vm.h"

#include "testutil.h"
#include "vm.h"
#include "vmstring.h"

#include <stdio.h>
#include <string.h>

const char *TAG_VM = "TEST_VM";

int testPushPop() {
    int failed = 0;

    VM *vm = vm_create();
    String *s1 = string_create();
    String *s2 = string_create_from("hello");
    
    //vm->debug = true;
    vm->code = (Instruction[]){
        {OP_PUSH, {.type = VAL_BOOL, .as.boolean = true}},
        {OP_PUSH, {.type = VAL_BOOL, .as.boolean = false}},
        {OP_PUSH, {.type = VAL_FLOAT, .as.floating = -25.0}},
        {OP_PUSH, {.type = VAL_FLOAT, .as.floating = 3.1415}},
        {OP_PUSH, {.type = VAL_FLOAT, .as.floating = 3.1415}},
        {OP_DISCARD, {}},
        {OP_PUSH, {.type = VAL_INTEGER, .as.integer = 0}},
        {OP_PUSH, {.type = VAL_INTEGER, .as.integer = 600}},
        {OP_PUSH, {.type = VAL_STRING, .as.string = s1}},
        {OP_PUSH, {.type = VAL_STRING, .as.string = s2}},
        {OP_HALT, {}}
    };
    vm_execute(vm);

    failed += test_assert(
        vm->stack[0].type == VAL_BOOL && vm->stack[0].as.boolean == true,
        TAG_VM,
        "Pushed true bool"
    );

    failed += test_assert(
        vm->stack[1].type == VAL_BOOL && vm->stack[1].as.boolean == false,
        TAG_VM,
        "Pushed false bool"
    );

    failed += test_assert(
        vm->stack[2].type == VAL_FLOAT && vm->stack[2].as.floating == -25.0,
        TAG_VM,
        "Pushed -25 float"
    );

    failed += test_assert(
        vm->stack[3].type == VAL_FLOAT && vm->stack[3].as.floating == 3.1415,
        TAG_VM,
        "Pushed 3.1415 float"
    );

    failed += test_assert(
        vm->stack[4].type == VAL_INTEGER && vm->stack[4].as.integer == 0,
        TAG_VM,
        "Popped second 3.1415 float, pushed 0 integer"
    );

    failed += test_assert(
        vm->stack[5].type == VAL_INTEGER && vm->stack[5].as.integer == 600,
        TAG_VM,
        "Pushed 600 integer"
    );

    failed += test_assert(
        vm->stack[6].type == VAL_STRING && strcmp(vm->stack[6].as.string->data, "") == 0,
        TAG_VM,
        "Pushed empty string"
    );

    failed += test_assert(
        vm->stack[7].type == VAL_STRING && strcmp(vm->stack[7].as.string->data, "hello") == 0,
        TAG_VM,
        "Pushed hello string"
    );

    string_free(s1);
    string_free(s2);
    vm_free(vm);

    return failed;
}

int testStrings() {
    int failed = 0;

    VM *vm = vm_create();
    String *s2 = string_create_from(" world");
    String *s3 = string_create_from("hello");
    String *s4 = string_create_from("hello world");
    
    //vm->debug = true;
    vm->code = (Instruction[]){
        {OP_PUSH, {.type = VAL_STRING, .as.string = s2}},
        {OP_DUP, {}},
        {OP_PUSH, {.type = VAL_STRING, .as.string = s3}},
        {OP_CONCATSTR, {}},
        {OP_DUP, {}},
        {OP_PUSH, {.type = VAL_STRING, .as.string = s4}},
        {OP_STR_EQ, {}},
        {OP_PUSH, {.type = VAL_STRING, .as.string = s2}},
        {OP_PUSH, {.type = VAL_STRING, .as.string = s3}},
        {OP_STR_EQ, {}},
        {OP_PUSH, {.type = VAL_INTEGER, .as.integer = 2}},
        {OP_PUSH, {.type = VAL_INTEGER, .as.integer = 6}},
        {OP_PUSH, {.type = VAL_STRING, .as.string = s4}},
        {OP_SUBSTR, {}},
        {OP_PUSH, {.type = VAL_STRING, .as.string = s4}},
        {OP_STRLEN, {}},
        {OP_HALT, {}}
    };
    vm_execute(vm);

    failed += test_assert(
        vm->stack[0].type == VAL_STRING && strcmp(vm->stack[0].as.string->data, " world") == 0,
        TAG_VM,
        "First string world"
    );

    failed += test_assert(
        vm->stack[1].type == VAL_STRING && strcmp(vm->stack[1].as.string->data, "hello world") == 0,
        TAG_VM,
        "second string hello world"
    );

    failed += test_assert(
        vm->stack[2].type == VAL_BOOL && vm->stack[2].as.boolean == true,
        TAG_VM,
        "STR_EQ returned true for equal"
    );

    failed += test_assert(
        vm->stack[3].type == VAL_BOOL && vm->stack[3].as.boolean == false,
        TAG_VM,
        "STR_EQ returned false for non-equal"
    );

    failed += test_assert(
        vm->stack[4].type == VAL_STRING && strcmp(vm->stack[4].as.string->data, "wo") == 0,
        TAG_VM,
        "substring from hello world to wo"
    );

    failed += test_assert(
        vm->stack[5].type == VAL_INTEGER && vm->stack[5].as.integer == 11,
        TAG_VM,
        "STRLEN returned correct length"
    );

    string_free(s2);
    string_free(s3);
    string_free(s4);
    vm_free(vm);

    return failed;
}

int runVMTests() {
    int failed = 0;
    failed += testPushPop();
    failed += testStrings();

    if (failed > 0) {
        printf("%s: Tests failed: %d\n", TAG_VM, failed);
    }
    return failed;
}
