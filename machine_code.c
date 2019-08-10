#include "machine_code.h"
#include "file_processing.h"

machine_words *head = NULL;


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

void add_machine_word(machine_words* word) {
    machine_words* new_word;
    machine_words* latest_word_before;
    machine_words *iterator;

    if(!word)
        return;

    new_word = create_machine_word(word->address,word->value);

    if (!head) {
        head = new_word;
        return;
    }
    iterator = head;

    /* If needed to insert in head */
    if(word->address < iterator->address)
    {
        new_word->next = head;
        head = new_word;
        return;
    }
    latest_word_before;
    while (iterator && word->address > iterator->address) {
        latest_word_before = iterator;
        iterator = iterator->next;
    }

    if(!iterator)
    {
        latest_word_before->next=new_word;
        return;
    }
    new_word->next = latest_word_before->next;
    latest_word_before->next = new_word;
}

void add_machine_word_directive(machine_word_instruction word) {
    add_machine_word(get_word_int(word));
}

char *get_word_string(machine_word_instruction word) {
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

machine_words *get_word_int(machine_word_instruction word) {
    /* TODO: finish this function */
    return 0;

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
    printf ("Address\tValue\n");
    while (iterator!=NULL)
    {
       printf ("%d\t%d\n" ,iterator->address,iterator->value);
       iterator=iterator->next;
    }

    return;
}

machine_words *create_string_words(char *line) {
    char c;
    int count = 0;
    machine_words* head_word=NULL;
    machine_words* buffer;

    while(c = *line , c != '\"')
    {
        count++;
        /*head_word = malloc(sizeof(machine_words));*/

        buffer = head_word;
        head_word = create_machine_word(DC, c);
        head_word->next = buffer;

        DC++;
        line++;
    }

    return head_word;
}

machine_words* create_machine_word(int address, int value)
{
    machine_words* new_word = (machine_words*) malloc(sizeof(machine_words));
    new_word->value=value;
    new_word->address=address;
    return new_word;
}

machine_words *create_number_words(char *line) {

    int new_num_data;
    machine_words* head_word=NULL;
    machine_words* buffer= NULL;
    bool there_is_data = TRUE;
    new_num_data = 0;

    while(line = strip_number_or_label(line,&new_num_data), line != NULL)
    {

        buffer = head_word;
        head_word = create_machine_word(DC, new_num_data);
        DC++;
        head_word->next = buffer;
        line = strip_blank_chars(line);
        if(!*line)
        {
        /* If end of the line */
        return head_word;
        }
        if(*line != ',' && !isspace(*line))
        {
            print_error(BAD_COMMAND_ARGUMENT,lines_count);
            return NULL;
        }
        if(*line == ',')
        {
            line++;
            line=strip_blank_chars(line);
        }
    }

    return head_word;
}


int get_number_of_instruction_words(char *line, machine_word_instruction *first_word, opcode_p opcode) {
    int word_count = 1;
    int argument_count;
    char c;
    char argument[LABEL_MAX_SIZE];
    int i;
    char operands[2][LABEL_MAX_SIZE];/* 0 - source operand, 1 dest operand */

    line = strip_blank_chars(line);
    for(i=0 ; line = strip_operand_chars(line,operands[i]) , *line && argument; i++)
    {
        line = strip_blank_chars(line);
        if(*line == OPERAND_SEPERATOR)
        {
            line++;
            line = strip_blank_chars(line);
        }
        else if (*line)
        {
            print_error(SYNTAX_ERROR,lines_count);
            return 0;
        }
        else
        {
            break;
        }
    }

    first_word->opcode = opcode->code;
    first_word->address_operand_src = get_operand_addressing_method(operands[0]);
    first_word->address_operand_dest = get_operand_addressing_method(operands[1]);
    print_machine_word(*first_word);
    fflush(stdout);

    if((unsigned int)first_word->address_operand_src == DIRECT_REGISTER && (unsigned int)first_word->address_operand_dest == DIRECT_REGISTER)
        word_count+=1;

    return 0;
}

