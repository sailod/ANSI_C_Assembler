/*
 Created by Roi Zairi on 20/7/19.
*/
/* TODO: create externals table for output file at the end */

#include "extern_table.h"
#include "file_processing.h"

externals_table *extern_head = NULL;

void add_to_externals(char *name, int address) {
    externals_table *new = (externals_table *) malloc(sizeof(externals_table));
    char *external_name = malloc(sizeof(name));
    strcpy(external_name, name);
    new->address = address;
    new->external_name = strcpy(external_name,name);
    new->next = NULL;
    externals_table **iterator = &extern_head;
    if (!*iterator)
        *iterator = new;
    else {
        while (*iterator) {
            if (!((*iterator)->next)) {
                (*iterator)->next = new;
                return;;
            }
            *iterator = (*iterator)->next;
        }

    }
}