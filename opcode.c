#include "opcode.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "error_handling.h"
#include "file_processing.h"

Opcode *hash_table[OPCODES_COUNT] = {NULL};

Opcode mov = {"mov", 0, OPS_2};
Opcode cmp = {"cmp", 1, OPS_2};
Opcode add = {"add", 2, OPS_2};
Opcode sub = {"sub", 3, OPS_2};
Opcode not = {"not", 4, OP_1};
Opcode clr = {"clr", 5, OP_1};
Opcode lea = {"lea", 6, OPS_2};
Opcode inc = {"inc", 7, OP_1};
Opcode dec = {"dec", 8, OP_1};
Opcode jmp = {"jmp", 9, OP_1};
Opcode bne = {"bne", 10, OP_1};
Opcode red = {"red", 11, OP_1};
Opcode prn = {"prn", 12, OP_1};
Opcode jsr = {"jsr", 13, OP_1};
Opcode rts = {"rts", 14, NO_OP};
Opcode stop = {"stop", 15, NO_OP};

int getHashIndex(char *name) {
    int sum;
    char c;
    while ((c = *(name++))) {
        sum += c;
    }
    return sum % OPCODES_COUNT;
}

void init_opcode_hash_table() {
    insert_opcode_node(&mov, 0);
    insert_opcode_node(&cmp, 0);
    insert_opcode_node(&add, 0);
    insert_opcode_node(&sub, 0);
    insert_opcode_node(&not, 0);
    insert_opcode_node(&clr, 0);
    insert_opcode_node(&lea, 0);
    insert_opcode_node(&inc, 0);
    insert_opcode_node(&dec, 0);
    insert_opcode_node(&jmp, 0);
    insert_opcode_node(&bne, 0);
    insert_opcode_node(&red, 0);
    insert_opcode_node(&prn, 0);
    insert_opcode_node(&jsr, 0);
    insert_opcode_node(&rts, 0);
    insert_opcode_node(&stop, 0);
}

void insert_opcode_node(Opcode *node, int index) {
    Opcode *temp;
    if (hash_table[index] == NULL) {
        hash_table[index] = node;
        node->next = NULL;
    } else {
        temp = hash_table[index];
        for (int i = 0; i < OPCODES_COUNT - 1; ++i) {
            if (hash_table[i] == NULL) {
                hash_table[i] = node;
                node->next = NULL;
                return;
            }
        }
        temp->next = node;
        node->next = NULL;
    }
}

Opcode *get_opcode_node(char *op) {
    Opcode *temp = NULL;
    for (int i = 0; i < OPCODES_COUNT; ++i) {
        if(hash_table[i] && !strcmp(hash_table[i]->name,op))
            return hash_table[i];
    }

    if (temp == NULL) {
        print_error(INSTRUCTION_NOT_FOUND, lines_count);
        return NULL;
    }
}
