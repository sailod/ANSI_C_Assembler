#include "directives.h"
int find_directive_type(char *directive_name){
    directive_p dir_p = directives;
    while (dir_p->name){
        if (strcmp(dir_p->name, directive_name)==0){
            return dir_p->type;
        }
        dir_p++;
    }
    return NOT_EXISTS_DIRECTIVE_TYPE;
}