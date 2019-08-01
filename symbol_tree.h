#ifndef ASSEMBLER_SYMBOL_TREE_H
#define ASSEMBLER_SYMBOL_TREE_H


#define MAX_LABEL 50

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "utils.h"
#include "directives.h"

typedef struct Symbol *sym_pt;

typedef struct Symbol {
    char *label;
    int value; /* Address in case of label */
    int type;
    sym_pt left;
    sym_pt right;
} Symbol;

sym_pt create_symbol_node(char *label, int address, int type);
void insert_symbol_to_tree(sym_pt new_symbol);
void insert_symbol(sym_pt new_symbol, sym_pt *cur_node);
sym_pt search_label(char *label, sym_pt head);
void print_symbol_table_recursion(sym_pt head);
void print_symbol_table();


#endif


