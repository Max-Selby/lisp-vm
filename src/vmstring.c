#include "vmstring.h"

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

bool string_init(String *s) {
    if (!s) return false;
    s->len = 0;
    s->cap = 16;
    s->data = malloc(s->cap);
    if (!s->data) return false;
    s->data[0] = '\0';
    return true;
}

bool string_init_from(String *s, const char *text) {
    if (!s || !text) return false;
    if (!string_init(s)) {
        return false;
    }
    if (!string_append(s, text)) {
        string_free(s);
        return false;
    }
    return true;
}

bool string_append(String *s, const char *text) {
    if (!s || !text) return false;
    size_t add = strlen(text);
    if (s->len + add + 1 > s->cap) {
        if (add > SIZE_MAX - s->len - 1) return false;
        while (s->len + add + 1 > s->cap) {
            s->cap *= 2;
        }
        char *tmp = realloc(s->data, s->cap);
        if (!tmp) return false;
        s->data = tmp;
    }
    memcpy(s->data + s->len, text, add + 1);
    s->len += add;
    return true;
}

bool string_substr(String *s, size_t start, size_t length) {
    if (!s) return false;
    if (!s->data) return false;
    size_t srcLen = s->len;
    if (start >= srcLen || start + length > srcLen) return false;
    
    char *tmp = realloc(s->data, length + 1);
    if (!tmp) return false;

    s->data = tmp;
    memmove(s->data, s->data + start, length);
    s->cap = length + 1;
    s->data[length] = '\0';
    s->len = length;
    return true;
}

void string_free(String *s) {
    if (!s) return;
    free(s->data);
    s->data = NULL;
    s->len = 0;
    s->cap = 0;
}

bool string_equal(String *s1, String *s2, bool *equiv) {
    if (!s1 || !s2) return false;
    if (!s1->data || !s2->data) return false;
    *equiv = (strcmp(s1->data, s2->data) == 0);
    return true;
}

bool string_length(String *s, int *len) {
    if (!s || !s->data) return false;
    *len = (int)strlen(s->data);
    return true;    
}
