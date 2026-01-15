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
 * Initialize a String object
 * @param s pointer to the String object to init
 * @returns true on success, false on failure
 */
bool string_init(String *s);

/**
 * Initialize a String object with starting value
 * @param s Pointer to the String object to init
 * @param text Pointer to the text it should store
 * @returns true on success, false on failure
 */
bool string_init_from(String *s, const char *text);

/**
 * Append a text value to a String object
 * @param s Pointer to the String object
 * @param text Pointer to the text to append
 * @returns true on success, false on failure
 */
bool string_append(String *s, const char *text);

/**
 * Set a String object to a substring of itself
 * @param s Pointer to the String object
 * @param start Index to start at (inclusive)
 * @param length Length of the substring
 * @returns true on success, false on failure
 */
bool string_substr(String *s, size_t start, size_t length);

/**
 * Free the memory used by a String object
 * @param s Pointer to the String object
 */
void string_free(String *s);

#endif // VMSTRING_H

