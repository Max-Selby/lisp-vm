#include "codegen.h"

#include <string.h>
#include <stdio.h>

#include "vmstring.h"
#include "parser.h"
#include "vm.h"

void codegen_error(char *msg) {
    printf("Codegen error: %s\n", msg);
    exit(1);
}

SymbolTable* symbol_table_create() {
    SymbolTable *table = malloc(sizeof(SymbolTable));
    table->count = 0;
    table->capacity = 8;
    table->names = malloc(sizeof(String*) * table->capacity);
    table->locations = malloc(sizeof(int) * table->capacity);
    return table;
}

void symbol_table_free(SymbolTable *table) {
    for (int i = 0; i < table->count; i++) {
        string_free(table->names[i]);
    }
    free(table->names);
    free(table->locations);
    free(table);
}

int symbol_table_lookup(SymbolTable *table, String *name) {
    for (int i = 0; i < table->count; i++) {
        if (strcmp(table->names[i]->data, name->data) == 0) {
            return table->locations[i];
        }
    }

    // Not found
    return -1;
}

int symbol_table_define(SymbolTable *table, String *name) {
    int existing_loc = symbol_table_lookup(table, name);
    if (existing_loc != -1) {
        // Already defined
        return existing_loc;
    }

    // Check if we need to resize
    if (table->count >= table->capacity) {
        table->capacity *= 2;
        table->names = realloc(table->names, sizeof(String*) * table->capacity);
        table->locations = realloc(table->locations, sizeof(int) * table->capacity);
    }

    // Add new symbol
    table->names[table->count] = string_copy(name); // Copy the string for memory management purposes
    table->locations[table->count] = table->count;
    table->count++;
    return table->locations[table->count - 1];
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

// Helper to compile functions that take an exact number of arguments
static void codegen_function_exact_args(
    ASTNode *node,
    BytecodeBuf *bbuf,
    SymbolTable *symtable,
    OpCode opCode,
    char *func_name,
    size_t arg_count
) {
    if (node->list.count != (int)arg_count + 1) {
        char err_msg[256];
        sprintf(err_msg, "Function '%s' expects exactly %zu arguments\n", func_name, arg_count);
        codegen_error(err_msg);
    }

    // Compile all argument expressions
    for (int i = 1; i < node->list.count; i++) {
        codegen_compile_expr(node->list.children[i], bbuf, symtable);
    }

    // Add function opcode
    bytecode_emit(bbuf, (Instruction){opCode, {0}});
}

// Helper to compile functions that take two or more arguments
static void codegen_function_twoplus_args(
    ASTNode *node,
    BytecodeBuf *bbuf,
    SymbolTable *symtable,
    OpCode opCode,
    char *func_name
) {
    if (node->list.count < 3) {
        char err_msg[256];
        sprintf(err_msg, "Function '%s' expects at least 2 arguments\n", func_name);
        codegen_error(err_msg);
    }

    // Compile all argument expressions
    for (int i = 1; i < node->list.count; i++) {
        codegen_compile_expr(node->list.children[i], bbuf, symtable);
    }

    // Add enough + instructions to sum all arguments
    for (int i = 1; i < node->list.count - 1; i++) {
        bytecode_emit(bbuf, (Instruction){opCode, {0}});
    }
}

void codegen_function_call(ASTNode *node, BytecodeBuf *bbuf, SymbolTable *symtable) {
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


    //////////////////////////////////////////////////////////
    //////////////// Base functions supported ////////////////
    //////////////////////////////////////////////////////////

    // define (variable definition)
    if (strcmp(func_name->data, "define") == 0) {
        if (node->list.count != 3) {
            codegen_error("define expects exactly 2 arguments");
        }

        ASTNode *var_name_node = node->list.children[1];
        if (var_name_node->type != AST_SYMBOL) {
            codegen_error("define: first argument must be a symbol");
        }
        String *var_name = var_name_node->symbol;

        // Compile the value expression
        ASTNode *value_node = node->list.children[2];
        codegen_compile_expr(value_node, bbuf, symtable);

        // Define the variable in the symbol table
        int location = symbol_table_define(symtable, var_name);

        // Value will be on the stack at this point, time to store it
        Instruction store_insn;
        store_insn.opCode = OP_STORE_VAR;
        store_insn.operand.type = VAL_INTEGER;
        store_insn.operand.as.integer = location;
        bytecode_emit(bbuf, store_insn);

        return;
    }

    // do (sequence of expressions)
    else if (strcmp(func_name->data, "do") == 0) {
        if (node->list.count < 2) {
            codegen_error("do expects at least 1 argument");
        }

        // Compile all expressions in sequence
        for (int i = 1; i < node->list.count; i++) {
            codegen_compile_expr(node->list.children[i], bbuf, symtable);
        }
        // (The value of the last expression will be the result)
    }

    // while (loop)
    else if (strcmp(func_name->data, "while") == 0) {
        if (node->list.count < 3) {
            codegen_error("while expects at least 2 arguments");
        }

        // Remember loop start address
        int loop_start_addr = bbuf->count;

        // Compile condition expression
        codegen_compile_expr(node->list.children[1], bbuf, symtable);

        // Jump if false placeholder
        int jmp_false_insn_index = bbuf->count;
        bytecode_emit(bbuf, (Instruction){OP_HALT, {0}}); // Placeholder for jump address

        bytecode_emit(bbuf, (Instruction){OP_JMP_IF_FALSE, {0}});

        // Compile body
        for (int i = 2; i < node->list.count; i++) {
            codegen_compile_expr(node->list.children[i], bbuf, symtable);
        }

        // Jump back to loop start
        bytecode_emit(bbuf, (Instruction){OP_PUSH, {.type = VAL_INTEGER, .as.integer = loop_start_addr}});
        bytecode_emit(bbuf, (Instruction){OP_JMP, {0}});

        // Fix up the jump false instruction to jump here
        int end_addr = bbuf->count;
        bbuf->instructions[jmp_false_insn_index].opCode = OP_PUSH;
        bbuf->instructions[jmp_false_insn_index].operand.type = VAL_INTEGER;
        bbuf->instructions[jmp_false_insn_index].operand.as.integer = end_addr;
    }

    // + (addition)
    else if (strcmp(func_name->data, "+") == 0) {
        codegen_function_twoplus_args(node, bbuf, symtable, OP_ADD, "+");
    }

    // - (subtraction)
    else if (strcmp(func_name->data, "-") == 0) {
        codegen_function_exact_args(node, bbuf, symtable, OP_SUB, "-", 2);
    }

    // * (multiplication)
    else if (strcmp(func_name->data, "*") == 0) {
        codegen_function_twoplus_args(node, bbuf, symtable, OP_MUL, "*");
    }

    // / (division)
    else if (strcmp(func_name->data, "/") == 0) {
        codegen_function_exact_args(node, bbuf, symtable, OP_DIV, "/", 2);
    }

    // % (modulo)
    else if (strcmp(func_name->data, "%") == 0) {
        codegen_function_exact_args(node, bbuf, symtable, OP_MOD, "%", 2);
    }

    // and (logic)
    else if (strcmp(func_name->data, "and") == 0) {
        codegen_function_twoplus_args(node, bbuf, symtable, OP_LOGIC_AND, "and");
    }

    // or (logic)
    else if (strcmp(func_name->data, "or") == 0) {
        codegen_function_twoplus_args(node, bbuf, symtable, OP_LOGIC_OR, "or");
    }

    // not (logic)
    else if (strcmp(func_name->data, "not") == 0) {
        codegen_function_exact_args(node, bbuf, symtable, OP_LOGIC_NOT, "not", 1);
    }

    // print
    else if (strcmp(func_name->data, "print") == 0) {
        codegen_function_exact_args(node, bbuf, symtable, OP_PRINT, "print", 1);
    }

    // println
    else if (strcmp(func_name->data, "println") == 0) {
        codegen_function_exact_args(node, bbuf, symtable, OP_PRINTLN, "println", 1);
    }

    // concat (string concatenation)
    else if (strcmp(func_name->data, "concat") == 0) {
        codegen_function_twoplus_args(node, bbuf, symtable, OP_CONCATSTR, "concat");
    }

    // substr (substring)
    else if (strcmp(func_name->data, "substr") == 0) {
        codegen_function_exact_args(node, bbuf, symtable, OP_SUBSTR, "substr", 3);
    }

    // == (numerical equality)
    else if (strcmp(func_name->data, "==") == 0) {
        codegen_function_exact_args(node, bbuf, symtable, OP_EQ, "==", 2);
    }

    // != (numerical inequality)
    else if (strcmp(func_name->data, "!=") == 0) {
        codegen_function_exact_args(node, bbuf, symtable, OP_NEQ, "!=", 2);
    }

    // < (numerical less than)
    else if (strcmp(func_name->data, "<") == 0) {
        codegen_function_exact_args(node, bbuf, symtable, OP_LT, "<", 2);
    }

    // <= (numerical less than or equal)
    else if (strcmp(func_name->data, "<=") == 0) {
        codegen_function_exact_args(node, bbuf, symtable, OP_LTE, "<=", 2);
    }

    // > (numerical greater than)
    else if (strcmp(func_name->data, ">") == 0) {
        codegen_function_exact_args(node, bbuf, symtable, OP_GT, ">", 2);
    }

    // >= (numerical greater than or equal)
    else if (strcmp(func_name->data, ">=") == 0) {
        codegen_function_exact_args(node, bbuf, symtable, OP_GTE, ">=", 2);
    }

    // str= (string equality)
    else if (strcmp(func_name->data, "str=") == 0) {
        codegen_function_exact_args(node, bbuf, symtable, OP_STR_EQ, "str=", 2);
    }

    // strlen (string length)
    else if (strcmp(func_name->data, "strlen") == 0) {
        codegen_function_exact_args(node, bbuf, symtable, OP_STRLEN, "strlen", 1);
    }

    // int2float
    else if (strcmp(func_name->data, "int2float") == 0) {
        codegen_function_exact_args(node, bbuf, symtable, OP_INT2FLOAT, "int2float", 1);
    }

    // float2int
    else if (strcmp(func_name->data, "float2int") == 0) {
        codegen_function_exact_args(node, bbuf, symtable, OP_FLOAT2INT, "float2int", 1);
    }



    // Unsupported function
    else {
        codegen_error("Unsupported function call");
    }
}

void codegen_compile_expr(ASTNode *node, BytecodeBuf *bbuf, SymbolTable *symtable) {
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

        // Symbols (lone symbol = variable load)
        case AST_SYMBOL: {
            int var_location = symbol_table_lookup(symtable, node->symbol);
            if (var_location == -1) {
                char err_msg[256];
                snprintf(err_msg, sizeof(err_msg), "Undefined variable: %s\n", node->symbol->data);
                codegen_error(err_msg);
            }

            bytecode_emit(
                bbuf, 
                (Instruction){
                    OP_LOAD_VAR, 
                    {
                        .type = VAL_INTEGER, 
                        .as.integer = var_location
                    }
                }
            );
            break;
        }

        // Function calls (lists)
        case AST_LIST: {
            codegen_function_call(node, bbuf, symtable);
            break;
        }
    }
}

void codegen_compile(ASTProgram *program, BytecodeBuf *bbuf, SymbolTable *symtable) {
    for (int i = 0; i < program->count; i++) {
        codegen_compile_expr(program->expressions[i], bbuf, symtable);
    }
    bytecode_emit(bbuf, (Instruction){OP_HALT, {0}});
}
