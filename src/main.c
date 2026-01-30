#include "lexer.h"
#include "vm.h"
#include "parser.h"
#include "vmstring.h"
#include "codegen.h"
#include "file_util.h"

#include <stdio.h>
#include <stdbool.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <filepath>\n", argv[0]);
        return 1;
    }

    char *source = file_read_all(argv[1]);
    if (!source) {
        printf("Error: Unable to read file %s\n", argv[1]);
        return 1;
    }

    Lexer *lexer = lexer_create(source);
    Parser *parser = parser_create(lexer);
    ASTProgram *program = parser_parse(parser);
    BytecodeBuf *bbuf = bytecode_create();
    codegen_compile(program, bbuf);

    VM *vm = vm_create();
    vm->code = bbuf->instructions;
    vm_execute(vm);
    
    astprogram_free(program);
    bytecode_free(bbuf);
    parser_free(parser);
    lexer_free(lexer);
    vm_free(vm);
    free(source);
}
