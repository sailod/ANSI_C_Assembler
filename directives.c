#include "directives.h"
int find_directive_type(char *directive_name){
    directive_p dir_p = directives;

    for (int i = 0; i < sizeof(directives) / sizeof(directive); ++i) {
        if (strcmp(directives[i].name, directive_name)==0){
            return directives[i].type;
        }
    }

    return NOT_EXISTS_DIRECTIVE_TYPE;
}