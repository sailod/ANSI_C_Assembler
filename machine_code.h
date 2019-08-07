#ifndef ASSEMBLER_MACHINE_CODE_H
#define ASSEMBLER_MACHINE_CODE_H


#include <stdio.h>
#include "error_handling.h"
#include "symbol_tree.h"
#include "utils.h"
#include <ctype.h>
#include <string.h>
#include "keywords.h"
#include "opcode.h"

#define WORD_SIZE 14

typedef struct machine_words {
    int address;
    int value;
    struct machine_words* next;
} machine_words;

typedef struct machine_word_directive {
    int a_r_e:2;
    int address_operand_dest:2;
    int address_operand_src:2;
    int opcode:4;
    int unused:4;
} machine_word_directive;

machine_words * head;
machine_words * tail;

/*
 * Declaring prototypes
 */

void print_machine_word(machine_word_directive word);
char* get_word(machine_word_directive word);
void add_machine_words(machine_words **words);
void print_data_machine_word(machine_words *pWords);

#endif
