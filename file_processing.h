#ifndef ASSEMBLER_FILE_PROCESSING_H
#define ASSEMBLER_FILE_PROCESSING_H


#include <stdio.h>
#include "error_handling.h"
#include "symbol_tree.h"
#include "utils.h"
#include <ctype.h>
#include <string.h>
#include "keywords.h"
#include "opcode.h"
#include "directives.h"
#include "machine_code.h"

#define MAX_CODE_LINE 81
#define WORD2_SIZE 10
#define COMMENT_CHAR ';'
#define LABEL_SEPARATOR ':'
#define LABEL_MAX_SIZE 50


typedef struct flags {
    unsigned is_label:1;
    unsigned in_data:1;
    unsigned in_mat:1;
} flags;

/*
 * Declaring prototypes
 */

bool search_entry(char label[50]);

bool search_extern(char label[50]);

int process_instruction_first_pass(char *line, int label);

void clean_data();

void clean_code();

void clean_entries_table();

void clean_externs_table();

void clean_symbol_table();

void generate_output_files(char *const filename);

int process_macro(char* line);

void process_file(char *filename);

void first_pass(FILE *fp);

int second_pass(FILE *fp);

short int is_comment_or_empty(char *line);

char *strip_blank_chars(char *line);

bool find_label(char *line, bool print_errors);

bool find_macro(char *line);

int process_line_first_pass(char *line);

int process_directive_first_pass(char *line, int is_label) ;

char *strip_label_chars(char *line, char label_name[LABEL_MAX_SIZE]);

char *strip_number(char *line, int* value);

char *strip_number_or_label(char *line, int *value);

char *strip_operand_chars(char *line, char label_name[LABEL_MAX_SIZE]);

int strip_operands(char *line, char operands[2][50]);

int process_data_or_string_line(char *line);

void add_symbol(char* label, int directive_type, int address);


/*
 * Done declaring prototypes.
 */


int IC, DC, err_count, lines_count;
char label[MAX_LABEL];

#endif
