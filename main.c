#include <stdio.h>
#include "file_processing.h"
#include "opcode.h"
#include "keywords.h"
#include "machine_code.h"

#define UASGE_MESSAGE "Usage: assemble an assembly code. by files provided in the terminal."

int test();

int main(int argc, char **argv) {

    char *file_name;
    test((*argv)+1);



    if (argc == 1) {
        printf(UASGE_MESSAGE);
    }
    init_opcode_hash_table();
    init_keywords_list();
    while (--argc) {
        file_name = *++argv;
        process_file(strcat(file_name, ASSEMBLY_FILE_EXTEN));
    }
}

int test()
{
   /* bool test=(bool)(1);
    printf("%d",test);*/
    bool test = (bool)(1 == 3);
   machine_word_instruction x = {2, 2, 2, 2, 0};
   char* ttttes = parse_word_string_represntation(x);
    return 0;
}
