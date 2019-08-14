#ifndef ASSEMBLER_OPCODE_H
#define ASSEMBLER_OPCODE_H
#include <stdlib.h>

#define OPCODES_COUNT 16
#define MAX_OPCODE_NAME 10
#define MAX_OPCODE_BITS 4

typedef enum {
    OPS_2, OP_1, NO_OP
} op_type;


typedef struct Opcode *opcode_p;

typedef struct Opcode {             /* Opcode node. it will be in an hash list, and opcodes with the same hash will be in a linked list */
    char name[MAX_OPCODE_NAME];
    int code;
    op_type type;
    opcode_p next;

} Opcode;

extern Opcode *hash_table[OPCODES_COUNT];

void init_opcode_hash_table();

void insert_opcode_node(Opcode *node, int index);

Opcode *get_opcode_node(char *op);

#endif /* ASSEMBLER_OPCODE_H */
