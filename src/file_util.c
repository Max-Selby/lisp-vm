#include "file_util.h"

#include <stdio.h>
#include <stdlib.h>

char *file_read_all(const char *filepath) {
    FILE *f = fopen(filepath, "r");
    if (!f) return NULL;

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    rewind(f);

    char *buffer = malloc(size + 1);
    if (!buffer) {
        fclose(f);
        return NULL;
    }

    fread(buffer, 1, size, f);
    buffer[size] = '\0';  // null-terminate

    fclose(f);
    return buffer;
}
