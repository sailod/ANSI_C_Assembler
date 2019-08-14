#include "directives.h"

directive directives[] = {
        {"data",   DATA_DIRECTIVE_TYPE},
        {"code",    CODE_DIRECTIVE_TYPE},
        {"string", STRING_DIRECTIVE_TYPE},
        {"entry",  ENTRY_DIRECTIVE_TYPE},
        {"extern", EXTERN_DIRECTIVE_TYPE},
        {"define", MACRO_DIRECTIVE_TYPE}
};
int find_directive_type(char *directive_name){
    int i;
    for (i = 0; i < sizeof(directives) / sizeof(directive); ++i) {
        if (strcmp(directives[i].name, directive_name)==0){
            return directives[i].type;
        }
    }

    return NOT_EXISTS_DIRECTIVE_TYPE;
}