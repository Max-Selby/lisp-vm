#include "codegen.h"

#include <string.h>
#include <stdio.h>

#include "parser.h"
#include "vm.h"

void codegen_error(char *msg) {
    printf("Codegen error: %s\n", msg);
    exit(1);
}

BytecodeBuf* bytecode_create() {
    BytecodeBuf *buf = malloc(sizeof(BytecodeBuf));
    buf->count = 0;
    buf->cap = 8;
    buf->instructions = malloc(sizeof(Instruction) * buf->cap);
    return buf;
}

void bytecode_free(BytecodeBuf *bbuf) {
    free(bbuf->instructions);
    free(bbuf);
}

void bytecode_emit(BytecodeBuf *bbuf, Instruction insn) {
    if (bbuf->count >= bbuf->cap) {
        bbuf->cap *= 2;
        bbuf->instructions = realloc(bbuf->instructions, sizeof(Instruction) * bbuf->cap);
    }
    bbuf->instructions[bbuf->count++] = insn;
}

void codegen_function_call(ASTNode *node, BytecodeBuf *bbuf) {
    if (node->type != AST_LIST) {
        codegen_error("Expected AST_LIST node for function call");
    }
    
    // Empty lists not supported.
    if (node->list.count == 0) {
        codegen_error("Cannot compile empty function call");
    }

    ASTNode *func_node = node->list.children[0];
    if (func_node->type != AST_SYMBOL) {
        codegen_error("Expected function name to be a symbol");
    }
    String *func_name = func_node->symbol;

    //////////////// Base functions supported ////////////////

    // + (addition)
    if (strcmp(func_name->data, "+") == 0) {
        if (node->list.count < 3) {
            codegen_error("Function '+' expects at least 2 arguments");
        }

        // Compile all argument expressions
        for (int i = 1; i < node->list.count; i++) {
            codegen_compile_expr(node->list.children[i], bbuf);
        }

        // Add enough + instructions to sum all arguments
        for (int i = 1; i < node->list.count - 1; i++) {
            bytecode_emit(bbuf, (Instruction){OP_ADD, {0}});
        }
    }

    // - (subtraction)
    else if (strcmp(func_name->data, "-") == 0) {
        if (node->list.count != 3) {
            codegen_error("Function '-' expects exactly 2 arguments");
        }

        // Compile argument expressions
        codegen_compile_expr(node->list.children[1], bbuf);
        codegen_compile_expr(node->list.children[2], bbuf);

        // Subtract second from first
        bytecode_emit(bbuf, (Instruction){OP_SUB, {0}});
    }

    // * (multiplication)
    else if (strcmp(func_name->data, "*") == 0) {
        if (node->list.count < 3) {
            codegen_error("Function '*' expects at least 2 arguments");
        }

        // Compile all argument expressions
        for (int i = 1; i < node->list.count; i++) {
            codegen_compile_expr(node->list.children[i], bbuf);
        }

        // Add enough + instructions to sum all arguments
        for (int i = 1; i < node->list.count - 1; i++) {
            bytecode_emit(bbuf, (Instruction){OP_MUL, {0}});
        }
    }

    // / (division)
    else if (strcmp(func_name->data, "/") == 0) {
        if (node->list.count != 3) {
            codegen_error("Function '/' expects exactly 2 arguments");
        }

        // Compile argument expressions
        codegen_compile_expr(node->list.children[1], bbuf);
        codegen_compile_expr(node->list.children[2], bbuf);

        // Subtract second from first
        bytecode_emit(bbuf, (Instruction){OP_DIV, {0}});
    }

    // print
    else if (strcmp(func_name->data, "print") == 0) {
        if (node->list.count != 2) {
            codegen_error("Function 'print' expects exactly 1 argument");
        }

        // Compile the argument expression
        codegen_compile_expr(node->list.children[1], bbuf);

        // Emit the print instruction
        bytecode_emit(bbuf, (Instruction){OP_PRINT, {0}});
    }

    // println
    else if (strcmp(func_name->data, "println") == 0) {
        if (node->list.count != 2) {
            codegen_error("Function 'println' expects exactly 1 argument");
        }

        // Compile the argument expression
        codegen_compile_expr(node->list.children[1], bbuf);

        // Emit the print instruction
        bytecode_emit(bbuf, (Instruction){OP_PRINTLN, {0}});
    }

    else {
        codegen_error("Unsupported function call");
    }
}

void codegen_compile_expr(ASTNode *node, BytecodeBuf *bbuf) {
    switch (node->type) {

        // Literals
        case AST_INTEGER: {
            Instruction insn;
            insn.opCode = OP_PUSH;
            insn.operand.type = VAL_INTEGER;
            insn.operand.as.integer = node->integer;
            bytecode_emit(bbuf, insn);
            break;
        }
        case AST_FLOAT: {
            Instruction insn;
            insn.opCode = OP_PUSH;
            insn.operand.type = VAL_FLOAT;
            insn.operand.as.floating = node->floating;
            bytecode_emit(bbuf, insn);
            break;
        }
        case AST_BOOL: {
            Instruction insn;
            insn.opCode = OP_PUSH;
            insn.operand.type = VAL_BOOL;
            insn.operand.as.boolean = node->boolean;
            bytecode_emit(bbuf, insn);
            break;
        }
        case AST_STRING: {
            Instruction insn;
            insn.opCode = OP_PUSH;
            insn.operand.type = VAL_STRING;
            insn.operand.as.string = node->string;
            bytecode_emit(bbuf, insn);
            break;
        }

        // Symbols (lone symbol = variable, currently unsupported)
        case AST_SYMBOL: {
            codegen_error("Variable references not supported yet");
            break;
        }

        // Function calls (lists)
        case AST_LIST: {
            codegen_function_call(node, bbuf);
            break;
        }
    }
}

void codegen_compile(ASTProgram *program, BytecodeBuf *bbuf) {
    for (int i = 0; i < program->count; i++) {
        codegen_compile_expr(program->expressions[i], bbuf);
    }
    bytecode_emit(bbuf, (Instruction){OP_HALT, {0}});
}



// TODO: delete these ------------
// Instruction *codegen_generate(ASTProgram *program) {
    // TODO: VERY IMPORTANT: if calling codegen_generate_from_ast multiple times,
    // Remember to only copy over "insn_count" from each call, not the whole thing,
    // since it will likely include many unfilled instructions (garbage). Also if
    // codegen_generate_from_ast ends up appending OP_HALT always (which it probably should),
    // remember to delete that instruction from all but the last one.

    // Also, free the returned instructions arrays from codegen_generate_from_ast after copying.
// }
