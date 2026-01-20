#ifndef VM_H
#define VM_H

#include "vmstring.h"

#include <stdbool.h>

#define VM_STACK_SIZE (256)

typedef enum {
    OP_PUSH,        // Push value onto the stack
    OP_ADD,         // Pop two, push sum
    OP_SUB,         // Pop two, push second - first
    OP_MUL,         // Pop two, push product
    OP_DIV,         // Pop two, push second / first (runtime error if first = 0)
    OP_LOGIC_AND,   // Pop two, push a && b
    OP_LOGIC_OR,    // Pop two, push a || b
    OP_LOGIC_NOT,   // Pop a value, push !value
    OP_PRINT,       // Print top of stack without popping
    OP_CONCATSTR,   // Pop a & b, push string concatenation of a + b
    OP_SUBSTR,      // Pop a, b, c, push substring of a starting at b, of length c
    OP_DISCARD,     // Pop a value and do nothing with it
    OP_DUP,         // Pop a value and push it twice
    OP_SWAP,        // Pop two, push them so their order flips
    OP_EQ,          // Pop two numbers, push boolean first == second
    OP_NEQ,         // Pop two numbers, push boolean first != second
    OP_LT,          // Pop two numbers, push boolean first < second
    OP_LTE,         // Pop two numbers, push boolean first <= second
    OP_GT,          // Pop two numbers, push boolean first > second
    OP_GTE,         // Pop two numbers, push boolean first >= second
    OP_JMP,
    OP_JMP_IF,      
    OP_STR_EQ,      // Pop two, push boolean true if they are equivalent strings, else push false.
    OP_HALT         // Stop execution
} OpCode;

typedef enum {
    VAL_INTEGER,
    VAL_FLOAT,
    VAL_BOOL,
    VAL_STRING
} ValueType;

typedef struct {
    ValueType type;
    union {
        int integer;
        double floating;
        bool boolean;
        String *string;
    } as;
} Value;

typedef struct {
    OpCode opCode;
    Value operand;
} Instruction;

typedef struct {
    Value *stack;
    int stack_size;
    int sp; // Stack pointer
    Instruction *code;
    int pc; // Program counter
} VM;

VM* vm_create();
void vm_free(VM *vm);
void vm_execute(VM *vm);

#endif // VM_H

