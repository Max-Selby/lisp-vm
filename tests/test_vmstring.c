#include "test_vmstring.h"

#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "testutil.h"
#include "vmstring.h"

const char *TAG_VMSTRING = "VMSTRING";

int testGenerate() {
    int failed = 0;

    // Can create a string
    String s;
    failed += test_assert(
        string_init(&s),
        TAG_VMSTRING,
        "Initialize String"
    );
    failed += test_assert(
        strcmp(s.data, "") == 0,
        TAG_VMSTRING,
        "Ensure String holds value ''"
    );
    string_free(&s);

    // Can create a string from text
    failed += test_assert(
        string_init_from(&s, ""),
        TAG_VMSTRING,
        "Initialize String from empty string"
    );
    failed += test_assert(
        strcmp(s.data, "") == 0,
        TAG_VMSTRING,
        "Ensure String holds value ''"
    );
    string_free(&s);

    failed += test_assert(
        string_init_from(&s, "abc"),
        TAG_VMSTRING,
        "Initialize String from 'abc'"
    );
    failed += test_assert(
        strcmp(s.data, "abc") == 0,
        TAG_VMSTRING,
        "Ensure String holds value 'abc'"
    );
    string_free(&s);

    failed += test_assert(
        string_init_from(&s, "a"),
        TAG_VMSTRING,
        "Initialize String from 'a'"
    );
    failed += test_assert(
        strcmp(s.data, "a") == 0,
        TAG_VMSTRING,
        "Ensure String holds value 'a'"
    );
    string_free(&s);

    return failed;
}

int testAppend() {
    int failed = 0;
    String s;

    // Can append to existing
    string_init_from(&s, "abc");
    failed += test_assert(
        string_append(&s, "def"),
        TAG_VMSTRING,
        "Append 'def' to String 'abc'"
    );
    failed += test_assert(
        strcmp(s.data, "abcdef") == 0,
        TAG_VMSTRING,
        "Ensure String holds value 'abcdef'"
    );
    string_free(&s);

    // Can append to empty
    string_init(&s);
    failed += test_assert(
        string_append(&s, "def"),
        TAG_VMSTRING,
        "Append 'def' to String ''"
    );
    failed += test_assert(
        strcmp(s.data, "def") == 0,
        TAG_VMSTRING,
        "Ensure String holds value 'def'"
    );
    string_free(&s);

    return failed;
}

int testSubstring() {
    int failed = 0;
    String s;

    // Single character substring
    string_init_from(&s, "abc");
    failed += test_assert(
        string_substr(&s, 1, 1),
        TAG_VMSTRING,
        "Substring 'b' from String 'abc'"
    );
    failed += test_assert(
        strcmp(s.data, "b") == 0,
        TAG_VMSTRING,
        "Ensure String holds value 'b'"
    );
    string_free(&s);

    // Full substring
    string_init_from(&s, "abc");
    failed += test_assert(
        string_substr(&s, 0, 3),
        TAG_VMSTRING,
        "Substring 'abc' from String 'abc'"
    );
    failed += test_assert(
        strcmp(s.data, "abc") == 0,
        TAG_VMSTRING,
        "Ensure String holds value 'abc'"
    );
    string_free(&s);

    // Empty substring
    string_init_from(&s, "abc");
    failed += test_assert(
        string_substr(&s, 0, 0),
        TAG_VMSTRING,
        "Substring '' from String 'abc'"
    );
    failed += test_assert(
        strcmp(s.data, "") == 0,
        TAG_VMSTRING,
        "Ensure String holds value ''"
    );
    string_free(&s);

    // Bad arguments
    string_init_from(&s, "abc");
    failed += test_assert(
        string_substr(&s, 0, -1) == false,
        TAG_VMSTRING,
        "Negative substring length"
    );
    failed += test_assert(
        string_substr(&s, -1, 2) == false,
        TAG_VMSTRING,
        "Negative substring start"
    );
    failed += test_assert(
        string_substr(&s, 4, 0) == false,
        TAG_VMSTRING,
        "Substring start past string length"
    );
    failed += test_assert(
        string_substr(&s, 0, 4) == false,
        TAG_VMSTRING,
        "Full substring longer than string"
    );
    failed += test_assert(
        string_substr(&s, 1, 3) == false,
        TAG_VMSTRING,
        "Substring longer than string"
    );
    string_free(&s);
    string_init(&s);
    failed += test_assert(
        string_substr(&s, 0, 0) == false,
        TAG_VMSTRING,
        "Substring '' from String ''"
    );
    string_free(&s);

    return failed;
}

int runVMStringTests() {
    int failed = 0;
    failed += testGenerate();
    failed += testAppend();
    failed += testSubstring();

    if (failed > 0) {
        printf("%s: Tests failed: %d\n", TAG_VMSTRING, failed);
    }
    return failed;
}
