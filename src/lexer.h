#ifndef LEXER_H
#define LEXER_H

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
    char *value;
} Token;

typedef struct {
    char *input;
    int pos;
} Lexer;

Lexer* lexer_create(char *input);
Token lexer_next_token(Lexer *lexer);
void lexer_free(Lexer *lexer);

#endif // LEXER_H
