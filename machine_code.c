#include "file_processing.h"
#include <values.h>
#include "machine_code.h"

machine_words *head_instructions = NULL;
machine_words *head_data = NULL;

addressing_method immediate_addressing_method = {IMMEDIATE, 1};
addressing_method direct_addressing_method = {DIRECT, 1};
addressing_method permanent_index_addressing_method = {PERMANENT_INDEX, 2};
addressing_method direct_register_addressing_method = {DIRECT_REGISTER, 1};
addressing_method unknown_addressing_method = {UNKNOWN, 0};


void print_machine_word(machine_word_instruction word) {
    printf("\n%s", "0000");
    printf("%d", word.opcode & (1u << 3) ? 1 : 0);
    printf("%d", word.opcode & (1u << 2) ? 1 : 0);
    printf("%d", word.opcode & (1u << 1) ? 1 : 0);
    printf("%d", word.opcode & 1u);
    printf("%d", word.address_operand_src & (1u << 1) ? 1 : 0);
    printf("%d", word.address_operand_src & 1u);
    printf("%d", word.address_operand_dest & (1u << 1) ? 1 : 0);
    printf("%d", word.address_operand_dest & 1u);
    printf("%d", word.a_r_e & (1u << 1) ? 1 : 0);
    printf("%d", word.a_r_e & 1u);

}

void add_machine_word(machine_words *word, int type) {
    machine_words *new_word;
    machine_words *latest_word_before;
    machine_words *iterator;
    machine_words **head = (type == DATA_DIRECTIVE_TYPE || type == STRING_DIRECTIVE_TYPE) ? &head_data
                                                                                          : &head_instructions;

    if (!word)
        return;

    new_word = create_machine_word(word->address, word->value, word->desc, type);

    if (!*head) {
        *head = new_word;
        return;
    }
    iterator = *head;

    /* If needed to insert in head_instructions */
    if (word->address < iterator->address) {
        new_word->next = *head;
        *head = new_word;
        return;
    }

    while (iterator && word->address > iterator->address) {
        latest_word_before = iterator;
        iterator = iterator->next;
    }

    if (iterator && word->address == iterator->address) {
        printf("Address %d already in use, data = %d", iterator->address, iterator->value);
    }


    if (!iterator) {
        latest_word_before->next = new_word;
        return;
    }
    new_word->next = latest_word_before->next;
    latest_word_before->next = new_word;
}

char *parse_instruction_word_string_represntation(machine_word_instruction word) {
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
    *s = word.opcode & (1u << 3) ? '1' : '0';
    s++;
    *s = word.opcode & (1u << 2) ? '1' : '0';
    s++;
    *s = word.opcode & (1u << 1) ? '1' : '0';
    s++;
    *s = word.opcode & 1u ? '1' : '0';
    s++;
    *s = word.address_operand_src & (1u << 1) ? '1' : '0';
    s++;
    *s = word.address_operand_src & 1u ? '1' : '0';
    s++;
    *s = word.address_operand_dest & (1u << 1) ? '1' : '0';
    s++;
    *s = word.address_operand_dest & 1u ? '1' : '0';
    s++;
    *s = word.a_r_e & (1u << 1) ? '1' : '0';
    s++;
    *s = word.a_r_e & 1u ? '1' : '0';

    return s_head;
}

char *parse_word_string_represntation(machine_words word) {
    unsigned int iterator;
    int i;
    char *s = (char *) malloc(sizeof(char) * WORD_SIZE);
    char *s_head = s;

    for (i = 0; i < WORD_SIZE; ++i) {
        iterator = 1u << (WORD_SIZE - 1);
        iterator >>= i;

        *s = (iterator & word.value) ? '1' : '0';
        s++;
    }

    return s_head;
}

machine_words *parse_word_as_unsigned_int(machine_word_instruction word) {
    unsigned int new_word_unsigned_int = word.a_r_e;
    machine_words *new_machine_word;
    unsigned int temp = word.address_operand_dest;
    temp <<= 2;
    new_word_unsigned_int |= temp;

    temp = word.address_operand_src;
    temp <<= 4;
    new_word_unsigned_int |= temp;

    temp = word.opcode;
    temp <<= 6;
    new_word_unsigned_int |= temp;

    new_machine_word = (machine_words *) malloc(sizeof(machine_words));

    new_machine_word->value = new_word_unsigned_int;

    return new_machine_word;

}

void add_machine_words(machine_words *words, int type) {
    machine_words *new_words_iterator = words;

    while (new_words_iterator) {
        add_machine_word(new_words_iterator, type);
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
        head_word = create_machine_word(DC, c, line, STRING_DIRECTIVE_TYPE);
        head_word->next = buffer;

        DC++;
        line++;
    }
    buffer = head_word;
    head_word = create_machine_word(DC, 0, line, STRING_DIRECTIVE_TYPE);
    head_word->next = buffer;
    DC++;

    return head_word;
}

machine_words *create_machine_word(int address, int value, char *desc, int type) {
    machine_words *new_word = (machine_words *) malloc(sizeof(machine_words));
    new_word->value = value;
    new_word->address = address;
    new_word->desc = strndup_local(desc, MAX_CODE_LINE);
    new_word->type = type;
    return new_word;
}

machine_words *create_number_words(char *line) {

    int new_num_data;
    machine_words *head_word = NULL;
    machine_words *buffer = NULL;
    new_num_data = 0;

    while (line = strip_number_or_label(line, &new_num_data), line != NULL) {

        buffer = head_word;
        head_word = create_machine_word(DC, new_num_data, line, DATA_DIRECTIVE_TYPE);
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

int get_number_of_words(machine_words *head) {
    int count = 0;
    machine_words *iterator = head;
    while (iterator != NULL) {
        count++;
        iterator = iterator->next;
    }
    return count;
}

int get_number_of_instruction_words(char *line, machine_word_instruction *first_word, Opcode opcode) {
    int word_count = 1;
    char operands[2][LABEL_MAX_SIZE];/* 0 - source operand, 1 dest operand */
    int num_of_operands;
    addressing_method src_operand;
    addressing_method dest_operand;

    first_word->opcode = opcode.code;

    if (!(*line)) {
        return 1;
    }

    num_of_operands = strip_operands(line, operands);
    if (!num_of_operands) {
        return 1;
    }

    src_operand = get_operand_addressing_method(operands[0]);
    dest_operand = get_operand_addressing_method(operands[1]);

    first_word->address_operand_src = (num_of_operands == 1) ? dest_operand.method : src_operand.method;
    first_word->address_operand_dest = (num_of_operands == 1) ? src_operand.method : dest_operand.method;

    first_word->a_r_e = 0;

    if (DEBUG) {
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
    for (j = 0; line = strip_operand_chars(line, operands[j]), line; j++) {
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
    char temp[LABEL_MAX_SIZE];
    if (!(*string)) {
        return unknown_addressing_method;
    }
    if (string[0] == '#')
        return immediate_addressing_method;
    else if (strstr(string, "["))
        return permanent_index_addressing_method;
    else if (is_register(string) != NOT_REGISTER)
        return direct_register_addressing_method;
    else if (strip_label_chars(string, temp))
        return direct_addressing_method;
    else
        return unknown_addressing_method;

}

machine_words *get_machine_word_by_address(int address) {
    machine_words *iterator = head_instructions;
    while (iterator) {
        if (iterator->address == address)
            return iterator;
        iterator = iterator->next;
    }
    return NULL;
}

void update_data_addresses(sym_pt head, int last_IC) {
    machine_words *iterator = head_data;
    update_data_label_addresses(head, last_IC);

    while (iterator) {
        iterator->address += last_IC;
        iterator = iterator->next;
    }
}

void clean_data(machine_words *head) {
    if(!head)
        return;
    clean_data(head->next);
    free(head);
}