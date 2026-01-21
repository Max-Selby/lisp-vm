#include "lexer.h"
#include "vm.h"
#include "vmstring.h"

#include <stdio.h>
#include <stdbool.h>

// Testing to be removed
void tests1(void) {
    String s1, s2;
    string_init_from(&s2, "hello");
    string_init_from(&s1, "world\n");

    VM *vm = vm_create();
    vm->code = (Instruction[]){
        {OP_PUSH, {.type = VAL_STRING, .as.string = &s1}},
        {OP_PUSH, {.type = VAL_STRING, .as.string = &s2}},
        {OP_CONCATSTR, {0}},
        {OP_PRINT, {0}},
        {OP_HALT, {0}}
    };
    vm_execute(vm);

    // VM is not responsible for freeing strings, and so it never will
    string_free(&s2);
    string_free(&s1);
}

// This is all testing, should be removed later
int main(void) {
    Lexer *lexer = lexer_create("(+ 3 \"hi\") (defun foo (3.5 true false))");
    Token token;
    while ((token = lexer_next_token(lexer)).type != TOKEN_EOF) {
        printf("Token: %d\n", token.type);
        printf("Pos: %d\n", lexer->pos);
        if (token.type == TOKEN_INTEGER) printf("  Type : integer\n");
        else if (token.type == TOKEN_FLOAT) printf("  Type : float\n");
        else if (token.type == TOKEN_LPAREN) printf("  Type : (\n");
        else if (token.type == TOKEN_RPAREN) printf("  Type : )\n");
        else if (token.type == TOKEN_BOOL) printf("  Type : BOOL\n");
        else if (token.type == TOKEN_STRING) printf("  Type : string, val: %s\n", token.as.string->data);
        else if (token.type == TOKEN_SYMBOL) printf("  Type : symbol, val: %s\n", token.as.symbol->data);
        printf("=========\n");
    }
}
