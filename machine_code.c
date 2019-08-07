#include "machine_code.h"
#include "file_processing.h"

machine_words *head = NULL;

void print_machine_word(machine_word_directive word) {
    printf("\n%s", "0000");
    printf("%d", word.opcode & (1 << 3) ? 1 : 0);
    printf("%d", word.opcode & (1 << 2) ? 1 : 0);
    printf("%d", word.opcode & (1 << 1) ? 1 : 0);
    printf("%d", word.opcode & 1);
    printf("%d", word.address_operand_src & (1 << 1) ? 1 : 0);
    printf("%d", word.address_operand_src & 1);
    printf("%d", word.address_operand_dest & (1 << 1) ? 1 : 0);
    printf("%d", word.address_operand_dest & 1);
    printf("%d", word.a_r_e & (1 << 1) ? 1 : 0);
    printf("%d", word.a_r_e & 1);
}

void add_machine_word(machine_word_directive word) {
    machine_words *new = malloc(sizeof(machine_words));
    new->address = DC;
    machine_words *iterator;
    if (head == NULL) {
        head = new;
        return;
    }
    iterator = head;
    while (iterator->next != NULL) {
        iterator = iterator->next;
    }
    iterator->next = new;
}

char *get_word(machine_word_directive word) {
    char *s = (char *) malloc(sizeof(char) * WORD_SIZE);
    char *s_head = s;
    *s = '0';
    s++;
    *s = '0';
    s++;
    *s = '0';
    s++;
    *s = '0';
    s++;
    *s = word.opcode & (1 << 3) ? '1' : '0';
    s++;
    *s = word.opcode & (1 << 2) ? '1' : '0';
    s++;
    *s = word.opcode & (1 << 1) ? '1' : '0';
    s++;
    *s = word.opcode & 1 ? '1' : '0';
    s++;
    *s = word.address_operand_src & (1 << 1) ? '1' : '0';
    s++;
    *s = word.address_operand_src & 1 ? '1' : '0';
    s++;
    *s = word.address_operand_dest & (1 << 1) ? '1' : '0';
    s++;
    *s = word.address_operand_dest & 1 ? '1' : '0';
    s++;
    *s = word.a_r_e & (1 << 1) ? '1' : '0';
    s++;
    *s = word.a_r_e & 1 ? '1' : '0';

    return s_head;
}

void add_machine_words(machine_words **words) {
    print_data_machine_word(*words);
    machine_words *iterator=head;
    machine_words *latest_word_before = head;

    if(head == NULL && words != NULL)
{
    head=words;
}

    while(*words != NULL)
    {
        machine_words *new_word = *words;
print_data_machine_word(words);
        new_word->address= (*words)->address;
        new_word->value=(*words)->value;
        while(new_word->address > iterator->address  )
        {
            latest_word_before = iterator;
            iterator=iterator->next;
        }

        new_word->next = latest_word_before->next;
        latest_word_before->next = new_word;
    }


}
void print_data_machine_word(machine_words *words) {
    machine_words *iterator = words;
        while(iterator != NULL)
        {

            machine_words *new_word = malloc(sizeof(machine_words));

        printf("%d\n",iterator->address);
            iterator=iterator->next;
        }

}
