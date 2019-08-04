#include <stdio.h>
#include "file_processing.h"
#include "opcode.h"
#include "keywords.h"

#define UASGE_MESSAGE "Usage: assemble an assembly code. by files provided in the terminal."

int test();

int main(int argc, char **argv) {
    test((*argv)+1);
    if (argc == 1) {
        printf(UASGE_MESSAGE);
    }
    init_opcode_hash_table();
    init_keywords_list();
    while (--argc) {
        process_file(*++argv);
    }
}

int test()
{
   /* bool test=(bool)(1);
    printf("%d",test);*/
   bool test = (bool)(1 == 3);
    return 0;
}
