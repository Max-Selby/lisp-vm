#ifndef PARSER_H
#define PARSER_H

#include "vmstring.h"
#include "lexer.h"

typedef enum {
    AST_INTEGER,
    AST_FLOAT,
    AST_BOOL,
    AST_STRING,
    AST_SYMBOL,
    AST_LIST    // "List" = anything in parenthesis; (+ 1 2) is a list
} ASTNodeType;

typedef struct ASTNode {
    ASTNodeType type;
    union {
        int integer;
        double floating;
        bool boolean;
        String *string;
        String *symbol;
        struct {
            struct ASTNode **children;
            int count;
            int capacity;
        } list;
    };
} ASTNode;

typedef struct {
    ASTNode **expressions;
    int count;
    int capacity;
} ASTProgram;

typedef struct {
    Lexer *lexer;
    Token current_token;
} Parser;

/**
 * Creates a new parser using the given lexer
 */
Parser* parser_create(Lexer *lexer);

/**
 * Parses the tokens from the lexer into an ASTProgram
 */
ASTProgram* parser_parse(Parser *parser);

/**
 * Frees the given parser
 */
void parser_free(Parser *parser);

/**
 * Frees a single ASTNode and its children recursively
 */
void astnode_free(ASTNode *node);

/**
 * Prints the given ASTProgram
 * Useful for debugging
 */
void astprogram_print(ASTProgram *program);


#endif // PARSER_H
