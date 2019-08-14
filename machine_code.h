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
#include "file_processing.h"

#define WORD_SIZE 14
#define OPERAND_SEPERATOR ','
#define ERROR_WHEN_STRIP_OPERANDS -1
#define SRC_REGISTER 1
#define DEST_REGISTER 2

typedef struct machine_words {
    int address;
    int value;
    char* desc;/* stores the actual assembly line - debug purposes*/
    struct machine_words* next;
} machine_words;

typedef struct machine_word_instruction {
    unsigned int a_r_e:2;
    unsigned int address_operand_dest:2;
    unsigned int address_operand_src:2;
    unsigned int opcode:4;
    unsigned int unused:4;
} machine_word_instruction;

typedef struct machine_word_instruction_extra {
    int a_r_e:2;
    int data: 12;
} machine_word_instruction_extra;

typedef enum {
    IMMEDIATE = 0, DIRECT = 1, PERMANENT_INDEX = 2, DIRECT_REGISTER = 3, UNKNOWN = 20
} addressing_methods;

typedef struct addressing_method {
    addressing_methods method;
    int num_of_words;
} addressing_method;


machine_words * head;
machine_words * tail;

extern addressing_method immediate_addressing_method;
extern addressing_method direct_addressing_method;
extern addressing_method permanent_index_addressing_method;
extern addressing_method direct_register_addressing_method;
extern addressing_method unknown_addressing_method;

/*
 * Declaring prototypes
 */

void print_machine_word(machine_word_instruction word);
char* parse_instruction_word_string_represntation(machine_word_instruction word);
char *parse_word_string_represntation(machine_words word);
void add_machine_words(machine_words *words);
void print_data_machine_words(machine_words *words);

machine_words *create_string_words(char *line);
machine_words *create_machine_word(int address, int value, char *desc);
machine_words * parse_word_as_unsigned_int(machine_word_instruction word);
machine_words *create_number_words(char *line);
machine_words *get_machine_word_by_address(int address);
int get_number_of_words(machine_words* head);
/*
 * Gets the total number of words in memory that this instruction takes, and build the first word of it
 * */
int get_number_of_instruction_words(char *line, machine_word_instruction *first_word, Opcode opcode);

addressing_method get_operand_addressing_method(char string[50]);


#endif
