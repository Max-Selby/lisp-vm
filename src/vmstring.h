#ifndef VMSTRING_H
#define VMSTRING_H

#include <stdbool.h>
#include <stdlib.h>

typedef struct {
    char *data;
    size_t len;
    size_t cap; // Must include space for null terminator
} String;

/**
 * Initialize a string object
 * Sets s to NULL if an error occurs
 * @param s: pointer to the String object to init
 * @returns true on success, false on failure
 */
bool string_init(String *s);

/**
 * Initialize a string object with starting value
 * Sets s to NULL if an error occurs
 * @param s: Pointer to the String object to init
 * @param text: Pointer to the text it should store
 * @returns true on success, false on failure
 */
bool string_init_from(String *s, const char *text);

bool string_append(String *s, const char *text);

bool string_substr(String *s, size_t start, size_t length);

void string_free(String *s);

#endif // VMSTRING_H

