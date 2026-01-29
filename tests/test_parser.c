#include "test_parser.h"

#include <string.h>
#include <stdio.h>

#include "lexer.h"
#include "parser.h"
#include "testutil.h"

const char *TAG_PARSER = "TEST_PARSER";

static int test_basic() {
    int failed = 0;
    
    // This test assumes the lexer is working. If it's not, go fix that first.
    Lexer *lexer = lexer_create("(+ 3 \"hi\") (foo bar (3.5 true ()))");
    Parser *parser = parser_create(lexer);
    ASTProgram *program = parser_parse(parser);

    failed += test_assert(
        program->count == 2,
        TAG_PARSER,
        "Program has 2 expressions"
    );

    // Expression 1: (+ 3 "hi")
    failed += test_assert(
        program->expressions[0]->type == AST_LIST,
        TAG_PARSER,
        "First expression is a LIST"
    );

    ASTNode *firstExpr = program->expressions[0];
    failed += test_assert(
        firstExpr->list.count == 3,
        TAG_PARSER,
        "First expression has 3 children"
    );

    failed += test_assert(
        firstExpr->list.children[0]->type == AST_SYMBOL &&
        strcmp(firstExpr->list.children[0]->symbol->data, "+") == 0,
        TAG_PARSER,
        "First child of first expression is SYMBOL '+'"
    );
    failed += test_assert(
        firstExpr->list.children[1]->type == AST_INTEGER &&
        firstExpr->list.children[1]->integer == 3,
        TAG_PARSER,
        "Second child of first expression is INTEGER 3"
    );
    failed += test_assert(
        firstExpr->list.children[2]->type == AST_STRING &&
        strcmp(firstExpr->list.children[2]->string->data, "hi") == 0,
        TAG_PARSER,
        "Third child of first expression is STRING 'hi'"
    );

    // Expression 2: (foo bar (3.5 true ()))
    failed += test_assert(
        program->expressions[1]->type == AST_LIST,
        TAG_PARSER,
        "Second expression is a LIST"
    );

    ASTNode *secondExpr = program->expressions[1];
    failed += test_assert(
        secondExpr->list.count == 3,
        TAG_PARSER,
        "Second expression has 3 children"
    );

    failed += test_assert(
        secondExpr->list.children[0]->type == AST_SYMBOL &&
        strcmp(secondExpr->list.children[0]->symbol->data, "foo") == 0,
        TAG_PARSER,
        "First child of second expression is SYMBOL 'foo'"
    );
    failed += test_assert(
        secondExpr->list.children[1]->type == AST_SYMBOL &&
        strcmp(secondExpr->list.children[1]->symbol->data, "bar") == 0,
        TAG_PARSER,
        "Second child of second expression is SYMBOL 'bar'"
    );
    failed += test_assert(
        secondExpr->list.children[2]->type == AST_LIST,
        TAG_PARSER,
        "Third child of second expression is a LIST"
    );

    // Inner expression: (3.5 true ())
    ASTNode *innerList = secondExpr->list.children[2];
    failed += test_assert(
        innerList->list.count == 3,
        TAG_PARSER,
        "Inner list has 3 children"
    );

    failed += test_assert(
        innerList->list.children[0]->type == AST_FLOAT &&
        innerList->list.children[0]->floating == 3.5,
        TAG_PARSER,
        "First child of inner list is FLOAT 3.5"
    );
    failed += test_assert(
        innerList->list.children[1]->type == AST_BOOL &&
        innerList->list.children[1]->boolean == true,
        TAG_PARSER,
        "Second child of inner list is BOOL true"
    );
    failed += test_assert(
        innerList->list.children[2]->type == AST_LIST &&
        innerList->list.children[2]->list.count == 0,
        TAG_PARSER,
        "Third child of inner list is an empty LIST"
    );

    astprogram_free(program);
    parser_free(parser);
    lexer_free(lexer);

    return failed;
}

int run_parser_tests() {
    int failed = 0;
    failed += test_basic();

    if (failed > 0) {
        printf("%s: Tests failed: %d\n", TAG_PARSER, failed);
    }
    return failed;
}
