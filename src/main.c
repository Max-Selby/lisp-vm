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
    SymbolTable *symtable = symbol_table_create();
    codegen_compile(program, bbuf, symtable);

    VM *vm = vm_create();
    vm->code = bbuf->instructions;
    vm_execute(vm);

    astprogram_free(program);
    bytecode_free(bbuf);
    symbol_table_free(symtable);
    parser_free(parser);
    lexer_free(lexer);
    vm_free(vm);
    free(source);
}

// TODO:
/*
Todo list (in order of priority):
- Implement lists: [item1 item2 item3 ...]
- Implement local variables: (let [[var1 val1] [var2 val2]] expr)
- Implement control flow:
    - (if cond then else)   [DONE]
    - (while bool expr)     [DONE]
    - (repeat n expr)
    - (for var start end expr)  -- Make sure var is a local variable, scope is within the loop only.
- Implement functions: (defun name [args] body) and (name params)
- Implement (import "xyz")  <-- need to ensure argument is a string literal (variables can't work since imports not @ runtime)
    - Also (soft-import) that just returns false if file not found instead of erroring out.
- Implement REPL: make it so if you run lvm with no args it goes into REPL mode. Should be really easy.
- Implement something to check for overflow/underflow in math ops
    - This also includes parsing numbers from .mslisp files that are bigger than what can fit in int/double
- Implement support for file I/O
- Create libraries of common functions (math, string manipulation, etc.)
*/
