#include "test_lexer.h"

#include <string.h>
#include <stdio.h>

#include "lexer.h"
#include "testutil.h"

const char *TAG_LEXER = "TEST_LEXER";

int testBasic() {
    int failed = 0;
    Lexer *lexer = lexer_create("(+ 3 \"hi\" 53.2 true false \"\" \"\\\"\")");
    Token token;

    token = lexer_next_token(lexer);
    failed += test_assert(
        token.type == TOKEN_LPAREN,
        TAG_LEXER,
        "Token 1 is LPAREN"
    );

    token = lexer_next_token(lexer);
    failed += test_assert(
        token.type == TOKEN_SYMBOL &&
        strcmp(token.as.symbol->data, "+") == 0,
        TAG_LEXER,
        "Token 2 is SYMBOL '+'"
    );

    token = lexer_next_token(lexer);
    failed += test_assert(
        token.type == TOKEN_INTEGER &&
        token.as.integer == 3,
        TAG_LEXER,
        "Token 3 is INTEGER 3"
    );

    token = lexer_next_token(lexer);
    failed += test_assert(
        token.type == TOKEN_STRING &&
        strcmp(token.as.string->data, "hi") == 0,
        TAG_LEXER,
        "Token 4 is STRING 'hi'"
    );

    token = lexer_next_token(lexer);
    failed += test_assert(
        token.type == TOKEN_FLOAT &&
        token.as.floating == 53.2,
        TAG_LEXER,
        "Token 5 is FLOAT 53.2"
    );

    token = lexer_next_token(lexer);
    failed += test_assert(
        token.type == TOKEN_BOOL &&
        token.as.boolean == true,
        TAG_LEXER,
        "Token 6 is BOOLEAN true"
    );

    token = lexer_next_token(lexer);
    failed += test_assert(
        token.type == TOKEN_BOOL &&
        token.as.boolean == false,
        TAG_LEXER,
        "Token 7 is BOOLEAN false"
    );

    token = lexer_next_token(lexer);
    failed += test_assert(
        token.type == TOKEN_STRING &&
        strcmp(token.as.string->data, "") == 0,
        TAG_LEXER,
        "Token 8 is STRING ''"
    );

    token = lexer_next_token(lexer);
    failed += test_assert(
        token.type == TOKEN_STRING &&
        strcmp(token.as.string->data, "\"") == 0,
        TAG_LEXER,
        "Token 9 is STRING '\"'"
    );

    token = lexer_next_token(lexer);
    failed += test_assert(
        token.type == TOKEN_RPAREN,
        TAG_LEXER,
        "Token 10 is RPAREN"
    );

    token = lexer_next_token(lexer);
    failed += test_assert(
        token.type == TOKEN_EOF,
        TAG_LEXER,
        "Token 11 is EOF"
    );

    lexer_free(lexer);
    return failed;
}

int runLexerTests() {
    int failed = 0;
    failed += testBasic();

    if (failed > 0) {
        printf("%s: Tests failed: %d\n", TAG_LEXER, failed);
    }
    return failed;
}
