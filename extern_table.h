/*
 Created by Roi Zairi on 14/8/19.
*/
#ifndef ASSEMBLER_EXTERN_TABLE_H
#define ASSEMBLER_EXTERN_TABLE_H

#include <stdlib.h>
#include <string.h>

typedef struct externals_table {
    char* external_name;
    int address;
    struct externals_table* next;
} externals_table;

extern externals_table* extern_head;

void add_to_externals(char* name, int address);

#endif