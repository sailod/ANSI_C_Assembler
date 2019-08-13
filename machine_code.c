#include <values.h>
#include "machine_code.h"
#include "file_processing.h"

machine_words *head = NULL;

addressing_method immediate_addressing_method = { .method = IMMEDIATE, .num_of_words = 1};
addressing_method direct_addressing_method = { .method = DIRECT, .num_of_words = 1};
addressing_method permanent_index_addressing_method = { .method = PERMANENT_INDEX, .num_of_words = 2};
addressing_method direct_register_addressing_method = { .method = DIRECT_REGISTER, .num_of_words = 1};
addressing_method unknown_addressing_method = { .method = UNKNOWN, .num_of_words = 0};


void print_machine_word(machine_word_instruction word) {
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

void add_machine_word(machine_words *word) {
    machine_words *new_word;
    machine_words *latest_word_before;
    machine_words *iterator;

    if (!word)
        return;

    new_word = create_machine_word(word->address, word->value, word->desc);

    if (!head) {
        head = new_word;
        return;
    }
    iterator = head;

    /* If needed to insert in head */
    if (word->address < iterator->address) {
        new_word->next = head;
        head = new_word;
        return;
    }
    latest_word_before;
    while (iterator && word->address > iterator->address) {
        latest_word_before = iterator;
        iterator = iterator->next;
    }

    if(iterator && word->address == iterator->address)
    {
        printf("Address %d already in use, data = %d",iterator->address,iterator->value);
    }


    if (!iterator) {
        latest_word_before->next = new_word;
        return;
    }
    new_word->next = latest_word_before->next;
    latest_word_before->next = new_word;
}

void add_machine_word_instruction(machine_word_instruction word) {
    add_machine_word(parse_word_as_unsigned_int(word));
}

char *parse_word_string_represntation(machine_word_instruction word) {
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

machine_words *parse_word_as_unsigned_int(machine_word_instruction word) {
    /* TODO: finish this function */
    unsigned int new_word_unsigned_int =  word.a_r_e;
    unsigned int temp = word.address_operand_dest;
    temp <<=2 ;
    new_word_unsigned_int |= temp;

    temp = word.address_operand_src;
    temp <<=4;
    new_word_unsigned_int |= temp;

    temp = word.opcode;
    temp <<=6;
    new_word_unsigned_int |= temp;

    machine_words* new_machine_word = (machine_words*)malloc(sizeof(machine_words));

    new_machine_word->value = new_word_unsigned_int;

    return new_machine_word;

}

void add_machine_words(machine_words *words) {
    machine_words *exists_words_iterator = head;
    machine_words *new_words_iterator = words;

    machine_words *latest_word_before = head;

    while (new_words_iterator) {
        add_machine_word(new_words_iterator);
        new_words_iterator = new_words_iterator->next;
    }

    return;

}

void print_data_machine_words(machine_words *words) {


    machine_words *iterator = words;
    printf("Address\tValue\n");
    while (iterator != NULL) {
        printf("%d\t%d\n", iterator->address, iterator->value);
        iterator = iterator->next;
    }

    return;
}

machine_words *create_string_words(char *line) {
    char c;
    int count = 0;
    machine_words *head_word = NULL;
    machine_words *buffer;

    while (c = *line, c != '\"') {
        count++;
        /*head_word = malloc(sizeof(machine_words));*/

        buffer = head_word;
        head_word = create_machine_word(IC++, c, line);
        head_word->next = buffer;

        DC++;
        line++;
    }

    return head_word;
}

machine_words *create_machine_word(int address, int value, char *desc) {
    machine_words *new_word = (machine_words *) malloc(sizeof(machine_words));
    new_word->value = value;
    new_word->address = address;
    new_word->desc = strndup(desc,MAX_CODE_LINE);
    return new_word;
}

machine_words *create_number_words(char *line) {

    int new_num_data;
    machine_words *head_word = NULL;
    machine_words *buffer = NULL;
    bool there_is_data = TRUE;
    new_num_data = 0;

    while (line = strip_number_or_label(line, &new_num_data), line != NULL) {

        buffer = head_word;
        head_word = create_machine_word(IC, new_num_data, line);
        IC++;
        DC++;
        head_word->next = buffer;
        line = strip_blank_chars(line);
        if (!*line) {
            /* If end of the line */
            return head_word;
        }
        if (*line != ',' && !isspace(*line)) {
            print_error(BAD_COMMAND_ARGUMENT, lines_count);
            return NULL;
        }
        if (*line == ',') {
            line++;
            line = strip_blank_chars(line);
        }
    }

    return head_word;
}


int get_number_of_instruction_words(char *line, machine_word_instruction *first_word, Opcode opcode) {
    int word_count = 1;
    int argument_count;
    char c;
    char argument[LABEL_MAX_SIZE];
    int i;
    char operands[2][LABEL_MAX_SIZE];/* 0 - source operand, 1 dest operand */
    int num_of_operands;
    addressing_method src_operand;
    addressing_method dest_operand;
    int a_r_e = 0;

    if(!(*line))
    {
        return 1;
    }

    line = strip_blank_chars(line);
    num_of_operands = strip_operands(line, operands);

    first_word->opcode = opcode.code;

    src_operand = get_operand_addressing_method(operands[0]);
    dest_operand = get_operand_addressing_method(operands[1]);
    first_word->address_operand_src = src_operand.method;
    first_word->address_operand_dest = dest_operand.method;

    /* TODO: bring the real are value from first command*/
    first_word->a_r_e = a_r_e;

    if(DEBUG) {
        print_machine_word(*first_word);
        fflush(stdout);
    }
    if (first_word->address_operand_src == DIRECT_REGISTER && first_word->address_operand_dest == DIRECT_REGISTER)
        word_count++;
    else {
        word_count += src_operand.num_of_words;
        word_count += dest_operand.num_of_words;
    }
    return word_count;
}

int strip_operands(char *line, char operands[2][50]) {
    int j;
    for (j = 0; line = strip_operand_chars(line, operands[j]), *line; j++) {
        line = strip_blank_chars(line);
        if (*line == OPERAND_SEPERATOR) {
            line++;
            line = strip_blank_chars(line);
        } else if (*line) {
            print_error(SYNTAX_ERROR, lines_count);
            return ERROR_WHEN_STRIP_OPERANDS;
        }
    }
    return j;
}

addressing_method get_operand_addressing_method(char string[50]) {
    if(!(*string))
    {
        return unknown_addressing_method;
    }
    char temp[LABEL_MAX_SIZE];
    if (string[0] == '#')
        return immediate_addressing_method;
    else if (strstr(string, "["))
        return permanent_index_addressing_method;
    else if (is_register(string))
        return direct_register_addressing_method;
    else if (strip_label_chars(string, temp))
        return direct_addressing_method;
    else
        return unknown_addressing_method;

}

machine_words *get_machine_word_by_address(int address)
{
    machine_words* iterator = head;
    while(iterator)
    {
        if(iterator->address == address)
            return iterator;
        iterator=iterator->next;
    }
    return NULL;
}
