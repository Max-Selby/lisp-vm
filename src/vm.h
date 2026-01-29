#ifndef VM_H
#define VM_H

#include "vmstring.h"

#include <stdbool.h>

/**
 * OpCodes supported by the VM
 */
typedef enum {
    OP_PUSH,        // Push value onto the stack
    OP_ADD,         // Pop two, push sum
    OP_SUB,         // Pop two, push second - first
    OP_MUL,         // Pop two, push product
    OP_DIV,         // Pop two, push second / first (runtime error if first = 0)
    OP_MOD,         // Pop two, push second % first (integers only, runtime error if first = 0)
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
    OP_STR_EQ,      // Pop two, push boolean true if they are equivalent strings, else push false.
    OP_STRLEN,      // Pop a string, push its integer length
    OP_JMP,         // Pop an integer, jump to the instruction at that address
    OP_JMP_IF,      // Pop an integer then a bool. If the bool is true, jump.
    OP_INT2FLOAT,   // Pop an int, push float representation of its value
    OP_FLOAT2INT,   // Pop a float, push integer representation of its value
    OP_HALT         // Stop execution
} OpCode;

/**
 * Value types supported by the VM
 */
typedef enum {
    VAL_INTEGER,
    VAL_FLOAT,
    VAL_BOOL,
    VAL_STRING
} ValueType;

/**
 * A VM value
 */
typedef struct {
    ValueType type;
    union {
        int integer;
        double floating;
        bool boolean;
        String *string;
    } as;
} Value;

/**
 * A VM instruction
 */
typedef struct {
    OpCode opCode;
    Value operand;
} Instruction;

/**
 * The VM structure
 */
typedef struct {
    Value *stack;
    size_t stack_cap;
    int sp; // Stack pointer
    Instruction *code;
    int pc; // Program counter
    bool debug; // If true print debug info
    String **strings; // Strings in use by the VM
    size_t strings_count; // Number of strings in use
    size_t strings_cap; // Capacity of strings array
} VM;

/**
 * Creates a new VM instance
 */
VM* vm_create();

/**
 * Frees the given VM instance
 */
void vm_free(VM *vm);

/**
 * Executes the code loaded in the given VM
 */
void vm_execute(VM *vm);

#endif // VM_H
