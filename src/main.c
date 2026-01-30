#include "lexer.h"
#include "vm.h"
#include "parser.h"
#include "vmstring.h"
#include "codegen.h"

#include <stdio.h>
#include <stdbool.h>

// This is all testing, should be removed later
int main(void) {
    Lexer *lexer = lexer_create("(println (not true)) (println (not false))");

    Parser *parser = parser_create(lexer);
    ASTProgram *program = parser_parse(parser);
    //astprogram_print(program);

    BytecodeBuf *bbuf = bytecode_create();
    codegen_compile(program, bbuf);

    VM *vm = vm_create();
    vm->code = bbuf->instructions;
    vm_execute(vm);
    
    astprogram_free(program);
    parser_free(parser);
    lexer_free(lexer);
}
