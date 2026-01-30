#include "parser.h"

#include <stdio.h>

#include "vmstring.h"

// Forward declaration
ASTNode* parse_expr(Parser *parser);

void parser_error(char *msg) {
    printf("Parser error: %s\n", msg);
    exit(1);
}

Parser* parser_create(Lexer *lexer) {
    Parser *parser = malloc(sizeof(Parser));
    parser->lexer = lexer;
    parser->current_token = lexer_next_token(lexer);
    return parser;
}

void parser_advance(Parser *parser) {
    parser->current_token = lexer_next_token(parser->lexer);
}

ASTNode* parse_atom(Parser *parser) {
    Token token = parser->current_token;
    ASTNode *node = malloc(sizeof(ASTNode));

    switch (token.type) {
        case TOKEN_INTEGER: {
            node->type = AST_INTEGER;
            node->integer = token.as.integer;
            break;
        }
        case TOKEN_FLOAT: {
            node->type = AST_FLOAT;
            node->floating = token.as.floating;
            break;
        }
        case TOKEN_BOOL: {
            node->type = AST_BOOL;
            node->boolean = token.as.boolean;
            break;
        }
        case TOKEN_STRING: {
            node->type = AST_STRING;
            node->string = string_copy(token.as.string);
            break;
        }
        case TOKEN_SYMBOL: {
            node->type = AST_SYMBOL;
            node->symbol = string_copy(token.as.symbol);
            break;
        }
        default: {
            parser_error("Expected an atom but did not find one");
            break;
        }
    }

    parser_advance(parser);
    return node;
}

ASTNode* parse_list(Parser *parser) {
    if (parser->current_token.type != TOKEN_LPAREN) {
        parser_error("Expected '(' at start of list");
    }
    parser_advance(parser); // consume '('

    // Create list node
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = AST_LIST;
    node->list.count = 0;
    node->list.capacity = 4;
    node->list.children = malloc(sizeof(ASTNode*) * node->list.capacity);

    // Parse children until ')'
    while (parser->current_token.type != TOKEN_RPAREN) {
        if (parser->current_token.type == TOKEN_EOF) {
            parser_error("Unexpected EOF while parsing list");
        }

        // Make space for new child if needed
        if (node->list.count >= node->list.capacity) {
            node->list.capacity *= 2;
            ASTNode **tmp = realloc(
                node->list.children,
                sizeof(ASTNode*) * node->list.capacity
            );
            if (!tmp) {
                parser_error("Couldn't realloc children array for AST list");
            }
            node->list.children = tmp;
        }

        // Parse children recursively
        node->list.children[node->list.count++] = parse_expr(parser);
    }

    parser_advance(parser); // consume ')'
    return node;
}

// Parse a single expression (atom or list)
ASTNode* parse_expr(Parser *parser) {
    if (parser->current_token.type == TOKEN_EOF) {
        parser_error("Unexpected end of input");
    }
    
    if (parser->current_token.type == TOKEN_LPAREN) {
        // This will recursively call parse_expr for children
        return parse_list(parser);
    } else {
        return parse_atom(parser);
    }
}

ASTProgram* parser_parse(Parser *parser) {
    ASTProgram *program = malloc(sizeof(ASTProgram));
    program->count = 0;
    program->capacity = 4;
    program->expressions = malloc(sizeof(ASTNode*) * program->capacity);

    while (parser->current_token.type != TOKEN_EOF) {
        if (parser->current_token.type == TOKEN_RPAREN) {
            // This means there is a ) at top level, which doesn't make sense
            parser_error("Unmatched ')'");
        }

        // Make space for new ASTProgram if needed
        if (program->count >= program->capacity) {
            program->capacity *= 2;
            ASTNode **tmp = realloc(
                program->expressions,
                sizeof(ASTNode*) * program->capacity
            );
            if (!tmp) {
                parser_error("Couldn't realloc program array for AST");
            }
            program->expressions = tmp;
        }

        // Parse next top-level expression and add it to the program
        program->expressions[program->count++] = parse_expr(parser);
    }

    return program;
}

void astnode_free(ASTNode *node) {
    if (!node) return;

    if (node->type == AST_STRING) {
        string_free(node->string);
    } else if (node->type == AST_SYMBOL) {
        string_free(node->symbol);
    } else if (node->type == AST_LIST) {
        for (int i = 0; i < node->list.count; i++) {
            astnode_free(node->list.children[i]);
        }
        free(node->list.children);
    }
    free(node);
}

void astprogram_free(ASTProgram *program) {
    if (!program) return;

    for (int i = 0; i < program->count; i++) {
        astnode_free(program->expressions[i]);
    }
    free(program->expressions);
    free(program);
}

void parser_free(Parser *parser) {
    free(parser);
}

void astnode_print(ASTNode *node) {
    if (!node) return;

    switch (node->type) {
        case AST_INTEGER:
            printf("%d", node->integer);
            break;
        case AST_FLOAT:
            printf("%f", node->floating);
            break;
        case AST_BOOL:
            printf("%s", node->boolean ? "true" : "false");
            break;
        case AST_STRING:
            printf("\"%s\"", node->string->data);
            break;
        case AST_SYMBOL:
            printf("%s", node->symbol->data);
            break;
        case AST_LIST:
            printf("(");
            for (int i = 0; i < node->list.count; i++) {
                astnode_print(node->list.children[i]);
                if (i < node->list.count - 1) {
                    printf(" ");
                }
            }
            printf(")");
            break;
    }
}

void astprogram_print(ASTProgram *program) {
    printf("AST Program:\n");
    for (int i = 0; i < program->count; i++) {
        printf("Expression %d: ", i);
        astnode_print(program->expressions[i]);
        printf("\n");
    }
}
