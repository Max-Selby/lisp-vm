#include "lexer.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define DECIMAL_CHAR ('.')
#define QUOTE_CHAR ('"')
#define ESCAPE_CHAR ('\\')
#define BOOL_TRUE ("true")
#define BOOL_FALSE ("false")
#define LPAREN_CHAR ('(')
#define RPAREN_CHAR (')')

Lexer* lexer_create(char *input) {
    Lexer *lexer = malloc(sizeof(Lexer));
    lexer->input = input;
    lexer->pos = 0;
    return lexer;
}

void skip_whitespace(Lexer *lexer) {
    while (isspace(lexer->input[lexer->pos])) {
        lexer->pos++;
    }
}

void lexer_error(char *msg) {
    printf("Lexer error: %s", msg);
    exit(1);
}

Token lexer_next_token(Lexer *lexer) {
    skip_whitespace(lexer);

    Token token;
    char current = lexer->input[lexer->pos];

    if (current == '\0') {
        token.type = TOKEN_EOF;
        return token;
    }

    if (current == LPAREN_CHAR) {
        token.type = TOKEN_LPAREN;
        lexer->pos++;
        return token;
    }

    if (current == RPAREN_CHAR) {
        token.type = TOKEN_RPAREN;
        lexer->pos++;
        return token;
    }

    // Integers and floats
    if (isdigit(current)) {
        int start = lexer->pos;
        int decimals = 0;
        while (isdigit(lexer->input[lexer->pos]) || (DECIMAL_CHAR == lexer->input[lexer->pos])) {
            if (DECIMAL_CHAR == lexer->input[lexer->pos]) {
                decimals++;
            }
            lexer->pos++;
        }
        int len = lexer->pos - start;

        if (decimals == 0) {
            token.type = TOKEN_INTEGER;
            token.as.integer = atoi(strndup(&lexer->input[start], len));
        }
        else if (decimals == 1) {
            token.type = TOKEN_FLOAT;
            token.as.floating = atof(strndup(&lexer->input[start], len));
        }
        else {
            lexer_error("Invalid number format: too many decimal points!");
        }

        return token;
    }

    // Strings; backslash to escape quotes is supported
    if (current == QUOTE_CHAR) {
        int start = lexer->pos;
        bool escape = false;
        while (
            (lexer->input[lexer->pos] != QUOTE_CHAR || escape || (lexer->pos - start == 0))
        ) {
            escape = false;
            if (lexer->input[lexer->pos] == ESCAPE_CHAR) {
                escape = true;
            }
            else if (lexer->input[lexer->pos] == '\0') {
                lexer_error("Unterminated string");
            }
            lexer->pos++;
        }
        int len = lexer->pos - start;
        lexer->pos++; // So that it doesn't start on the ending quote for the next token
        token.type = TOKEN_STRING;
        
        if (len == 0) {
            String *str = malloc(sizeof(String));
            if (!string_init(str)) {
                lexer_error("Couldn't initialize string");
            }
            token.as.string = str;
            return token;
        }

        // Escapes: \n -> newline, \t -> tab, \\ -> \, \" -> ".
        char strbuf[len + 1];
        escape = false;
        int strindex = 0;
        for (int i = 0; i < len; i++) {
            char current = lexer->input[start + i];
            if (escape) {
                switch (current) {
                    case 'n': {
                        strbuf[strindex] = '\n';
                        break;
                    }
                    case 't': {
                        strbuf[strindex] = '\t';
                        break;
                    }
                    case ESCAPE_CHAR: {
                        strbuf[strindex] = ESCAPE_CHAR;
                        break;
                    }
                    case QUOTE_CHAR: {
                        strbuf[strindex] = QUOTE_CHAR;
                        break;
                    }
                    default: {
                        lexer_error("Undefined escape sequence");
                    }
                }
                escape = false;
                strindex++;
            }
            else if (current == ESCAPE_CHAR) {
                escape = true;
            }
            else if (current == QUOTE_CHAR) {
                // Ignore
            }
            else {
                strbuf[strindex] = current;
                escape = false;
                strindex++;
            }
        }
        strbuf[strindex] = '\0';

        token.type = TOKEN_STRING;
        String *str = malloc(sizeof(String));
        if (!string_init_from(str, strbuf)) {
            lexer_error("Couldn't initialize string");
        }
        token.as.string = str;
        return token;
    }

    // Booleans
    char *maybe_true = strndup(&lexer->input[lexer->pos], strlen(BOOL_TRUE));
    char *maybe_false = strndup(&lexer->input[lexer->pos], strlen(BOOL_FALSE));
    bool is_true = strcmp(maybe_true, BOOL_TRUE) == 0;
    bool is_false = strcmp(maybe_false, BOOL_FALSE) == 0;
    if (is_true || is_false) {
        token.type = TOKEN_BOOL;
        if (is_true) {
            token.as.boolean = true;
            lexer->pos += strlen(BOOL_TRUE);
        }
        else {
            token.as.boolean = false;
            lexer->pos += strlen(BOOL_FALSE);
        }
        return token;
    }

    // Anything else is a symbol
    int start = lexer->pos;
    while (
        !isspace(lexer->input[lexer->pos]) &&
        lexer->input[lexer->pos] != LPAREN_CHAR &&
        lexer->input[lexer->pos] != RPAREN_CHAR &&
        lexer->input[lexer->pos] != QUOTE_CHAR &&
        lexer->input[lexer->pos] != '\0'
    ) {
        lexer->pos++;
    }
    int len = lexer->pos - start;
    token.type = TOKEN_SYMBOL;
    String *str = malloc(sizeof(String));
    if (!string_init_from(str, strndup(&lexer->input[start], len))) {
        lexer_error("Couldn't initialize string");
    }
    token.as.symbol = str;
    return token;
}
