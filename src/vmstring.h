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

/**
 * Given two strings, set a boolean to true if
 * they are equal
 * @param s1 First string
 * @param s2 Second string
 * @param equiv True if equal, false otherwise
 * @returns **ERROR CODE, NOT THE ANSWER** True on success, false on failure
 */
bool string_equal(String *s1, String *s2, bool *equiv);

/**
 * Given a string, return its integer length
 * @param s The string
 * @param len The length to set
 * @returns True on success, false on failure
 */
bool string_length(String *s, int *len);

#endif // VMSTRING_H

