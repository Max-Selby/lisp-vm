#ifndef LEXER_H
#define LEXER_H

#include <stdbool.h>

#include "vmstring.h"

typedef enum {
    TOKEN_LPAREN,   // (
    TOKEN_RPAREN,   // )
    TOKEN_INTEGER,  // 123
    TOKEN_FLOAT,    // 123.4
    TOKEN_BOOL,     // true, false
    TOKEN_STRING,   // "Hi"
    TOKEN_SYMBOL,   // +, -, defun, etc.
    TOKEN_EOF
} TokenType;

typedef struct {
    TokenType type;
    union {
        int integer;
        double floating;
        bool boolean;
        String *string;
        String *symbol; // Symbol names need to be saved
    } as;
} Token;

typedef struct {
    char *input;
    int pos;
} Lexer;

/**
 * Creates a new lexer for the given input string
 */
Lexer* lexer_create(char *input);

/**
 * Returns the next token from the lexer
 */
Token lexer_next_token(Lexer *lexer);

/**
 * Frees the given lexer
 */
void lexer_free(Lexer *lexer);

#endif // LEXER_H
