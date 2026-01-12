#ifndef VM_H
#define VM_H

#define VM_STACK_SIZE (256)

typedef enum {
    OP_PUSH,        // Push constant onto the stack
    OP_ADD,         // Pop two, push sum
    OP_SUB,         // Pop two, push second - first
    OP_MUL,         // Pop two, push product
    OP_PRINT_INT,   // Print top of stack as integer, without popping
    OP_HALT         // Stop execution
} OpCode;

typedef struct {
    int *stack;
    int stack_size;
    int sp; // Stack pointer
    unsigned char *code;
    int pc; // Program counter
} VM;

VM* vm_create();
void vm_free(VM *vm);
void vm_execute(VM *vm);

#endif // VM_H

