#include "file_processing.h"
#include "machine_code.h"
#include "symbol_tree.h"

addressing_method immediate_addressing_method;
addressing_method direct_addressing_method;
addressing_method permanent_index_addressing_method;
addressing_method direct_register_addressing_method;
addressing_method unknown_addressing_method;

void process_line_second_pass(char *p);

void process_entry_line(char *line);

/* process the rest of the command's operands */
void process_instruction_second_pass(char *line, Opcode command);

machine_words *create_empty_word();

machine_words *create_instruction_extra_words(char *line, machine_word_instruction *pInstruction, int total_words,
                                              char operands[2][LABEL_MAX_SIZE]);

void process_operand(char string[50], machine_words *pWords);

/*
 * First Pass Algorithm
 * 1.  IC -> 0, DC -> 0.
 * 2.  Read line, if EOF: move to step 16.
 * 3.  Check if first field of line is a label, if yes: move to step 4, if not: move to step 5.
 * 4.  turn on 'is_label' flag.
 * 5.  Check if current field is a directive for storing data (.data / .mat / .string)?
 *     if yes: move to step 6, if not: move to step 8.
 *
 *
 ****** Dealing with a directive ***********
 *
 *
 * 6.  If 'is_label' flag is turned on, insert label to Symbol table as a data Symbol
 *     and insert the current value of DC as the address.
 * 7.  verify type of data, insert into data memory and update DC by the words count. Go back to step 2.
 * 8.  Is this a .extern or .entry? If yes: move to step 9, if not: move to step 11.
 * 9.  Is this a .extern? if yes, insert the label (operand of .extern directive) to the Symbol table with flag 'is_external'.
 * 10. Go back to step 2
 *
 *
 ****** Dealing with an instruction ***********
 *
 *
 * 11. If 'is_label' flag is turned on, insert label to Symbol table as a data Symbol
 *     and insert the current value of IC as the address.
 * 12. Look for the instruction name in the Instructions table.
 * 13. Check operands of the instruction and calculate the words count.
 *     build the code of the first word (The instruction itself).
 * 14. update IC by the words count.
 * 15. Go back to step 2.
 *
 *
 *************** Wrapping up *****************
 *
 *
 * 16. If errors were found, stop the processing of the file.
 * 17. Update the addresses of the Symbol which refers to data, by adding the value of IC.
 * 18. Start second pass.
 *
 * Done!
 */




void process_file(char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        perror("Error while opening file");
    }

    first_pass(fp);
    if (err_count)
        return;
    update_data_addresses(symbol_head);
    fp = fopen(filename, "r");
    IC = 0;
    second_pass(fp);
    if (err_count)
        return;
    generate_output_files();
    clean_symbol_table();
    clean_externs_table();
    clean_entries_table();
    clean_code();
    clean_data();
}


int second_pass(FILE *fp) {
    IC = 0;
    char line[MAX_CODE_LINE];
    char *line_p;
    line_p = line;
    int errors = 0;
    lines_count = 1;
    while (fgets(line, MAX_CODE_LINE, fp)) {
        line_p = line;
        lines_count++;

        line_p = strip_blank_chars(line_p);
        if (is_comment_or_empty(line_p))
            continue;
        process_line_second_pass(line_p);
    }

    return 0;
}


void generate_output_files() {

}

void clean_symbol_table() {

}

void clean_externs_table() {

}

void clean_entries_table() {

}

void clean_code() {

}

void clean_data() {

}

void first_pass(FILE *fp) {

    char line[MAX_CODE_LINE];
    lines_count = 1;
    char *line_p;
    line_p = line;
    while (fgets(line, MAX_CODE_LINE, fp)) {
        line_p = line;
        lines_count++;

        /*Printing the addresses of pointers*/
        /*printf("%p\n", (void *)line_p);
        printf("%p\n", (void *)line);*/

        line_p = strip_blank_chars(line_p);
        if (is_comment_or_empty(line_p))
            continue;
        process_line_first_pass(line_p);
    }

    if (DEBUG)
        print_symbol_table();

}


char *go_to_next_field(char *line) {
    while (!isspace(*line))
        line++;
    while (isspace(*line)) {
        line++;
    }
    return line;
}


int process_line_first_pass(char *line) {
    {


        /*char *p=line  ;*/
        int temp = 0;
        int is_label = 0;
        int is_macro = 0;

        if (!line)
            return 0;

        /*___________________________________*/


        /* first character can either be a dot or a letter. */
        if (!(isalpha(*line) || *line == '.')) {
            err_count++;
            print_error(SYNTAX_ERROR, lines_count);
            return 0;
        }


        is_macro = find_macro(line);
        if (is_macro) {
            if (process_macro(line))/* Found errors while adding macro */
            {
                err_count++;
                print_error(SYNTAX_ERROR, lines_count);
            }
            return 0;
        }


        /******** Dealing with Label *******************/
        is_label = find_label(line);
        if (is_label) {
            line = go_to_next_field(line);
        }

        if (line == NULL) {
            err_count++;
            print_error(LABEL_WITH_NO_DATA_ERROR, lines_count);
            return 0;
        }

        /* Looking for directive...-------------*/
        if (*line == '.') {
            line++; /* skip the '.' character */
            return process_directive_first_pass(line, is_label);
        }
            /*--------------------------------------*/

        else {
            return process_instruction_first_pass(line, is_label);
        }
    }
}

void process_line_second_pass(char *line) {
    int is_label = 0;
    char *directive_type_str;
    int i = 0;
    int directive_type;
    sym_pt entry_sym = NULL;
    char *line_head;

    /******** Dealing with Label *******************/
    is_label = find_label(line);
    if (is_label) {
        line = go_to_next_field(line);
    }

    if (*line == '.') {
        return;
    }

    line_head = line;

    while (!isspace(*line++)) {
        i++;
    }
    directive_type_str = strndup(line_head, i);
    directive_type = find_directive_type(directive_type_str);

    if (directive_type == ENTRY_DIRECTIVE_TYPE) {
        process_entry_line(line);
        return;
    }


    opcode_p opcode = get_opcode_node(directive_type_str);
    if (!opcode) {
        print_error(SYNTAX_ERROR, lines_count);
        return;
    } else {
        process_instruction_second_pass(line, *opcode);
    }


}

void process_instruction_second_pass(char *line, Opcode command) {
    machine_words *first_word;
    machine_words* current_word;
    machine_words *extra = NULL;
    machine_word_instruction *empty= (machine_word_instruction*)malloc(sizeof(machine_word_instruction));
    char *first_operand;
    int L,i;
    int num_of_operands;
    int num_of_total_words = get_number_of_instruction_words(line,empty,command);

    char operands[2][LABEL_MAX_SIZE];
    num_of_operands = strip_operands(line, operands);

    first_word = get_machine_word_by_address(IC);

    current_word = first_word->next;

    if(num_of_operands == 2 && get_operand_addressing_method(operands[0]).method == direct_addressing_method.method && get_operand_addressing_method(operands[1]).method == direct_addressing_method.method)
    {
        /* TODO: Build one extra word for two registers (fill current_word) */
    }
    else
    {
        for (i = 0; i < num_of_operands; ++i) {
            process_operand(operands[i],current_word);
        }
    }
    extra = create_instruction_extra_words(line, first_word, L, operands);

    add_machine_words(extra);

}

void process_operand(char string[50], machine_words *pWords) {
/*TODO: implement this method */
}

machine_words *create_instruction_extra_words(char *line, machine_word_instruction *pInstruction, int total_words,
                                              char operands[2][LABEL_MAX_SIZE]) {
    return NULL;
}

void process_entry_line(char *line) {
    /* TODO: complete this method*/
    char entry_name[LABEL_MAX_SIZE];

    strip_blank_chars(line);

    strip_label_chars(line, line);


}

int process_macro(char *line) {
    char *line_pt = line;
    char name[LABEL_MAX_SIZE] = "";
    int value;

    line_pt += 7; /* skip define characters */
    line_pt = strip_blank_chars(line_pt);
    line_pt = strip_label_chars(line_pt, name);
    line_pt = strip_blank_chars(line_pt);
    if (*line_pt != '=') {
        print_error(SYNTAX_ERROR, lines_count);
    }
    line_pt++;
    line_pt = strip_blank_chars(line_pt);
    line_pt = strip_number(line_pt, &value);

    if (*line_pt != '\n') {
        print_error(SYNTAX_ERROR, lines_count);
        return 0;
    }

    if (!is_already_exists_label(name)) {
        sym_pt sym = create_symbol_node(name, value, find_directive_type("define"));
        insert_symbol_to_tree(sym);
    }


    return 0;
}

int process_instruction_first_pass(char *line, int is_label) {
    if (DEBUG)
        printf("Processing Phase 1 instruction: \n\t%s\n", line);

    char instruction[MAX_OPCODE_BITS];
    int L;
    int i;

    if (is_label) {
        add_symbol(label, find_directive_type("code"), IC + 100);

    }
    if (!isalpha(*line)) {
        print_error(INSTRUCTION_NOT_FOUND, lines_count);
        return 0;
    }
    line = strip_label_chars(line, instruction);

    opcode_p opcode = get_opcode_node(instruction);
    if (!opcode)
        return 0;

    line = strip_blank_chars(line);

    machine_word_instruction *first_word = (machine_word_instruction *) malloc(sizeof(machine_word_instruction));

    L = get_number_of_instruction_words(line, first_word, *opcode);

    machine_words *word = parse_word_as_unsigned_int(*first_word);
    word->address = IC++;
    word->desc = line;
    add_machine_words(word);
    for (int i = 1; i < L; ++i) {
        word = create_empty_word();
        word->address = IC++;
        word->desc = line;
        add_machine_words(word);
    }

    return 0;
}

machine_words *create_empty_word() {
    return (machine_words *) malloc(sizeof(machine_words));
}

bool find_label(char *line) {
    char *p = line;
    bool is_label_found;
    int i, c;
    i = 0;
    while (*p && *p != LABEL_SEPARATOR) {
        p++;
        i++;
    }
    is_label_found = (bool) (*p == LABEL_SEPARATOR);
    if (is_label_found) {
        strncpy(label, line, i);
        label[i] = '\0';
        if (i > MAX_LABEL) {
            print_error(LABEL_TOO_LONG_ERROR, lines_count);
            return FALSE;
        }
        for (c = 0; c < i; c++) { /* checking label characters.*/
            if (!isdigit(line[c]) && !isalpha(line[c])) {
                print_error(LABEL_CONTAINS_INVALID_CHARACTER_ERROR, lines_count);
                return FALSE;
            }
        }
        if (!isalpha(*line)) { /* label must start with an alpha char */
            print_error(LABEL_FIRST_CHAR_IS_NOT_ALPHA_ERROR, lines_count);
            return FALSE;
        }
        if (is_already_exists_label(label))
            return FALSE;
    }
    return is_label_found;
}

bool find_macro(char *line) {
    char *p = line;
    bool is_macro_found;

    is_macro_found = (bool) ((int) strncmp(".define", p, 7) == 0);

    return is_macro_found;
}

bool search_extern(char label[50]) {
    sym_pt sym = search_label(label, symbol_head);
    return (bool) (sym != NULL && sym->type == EXTERN_DIRECTIVE_TYPE);
}

bool search_entry(char label[50]) {
    sym_pt sym = search_label(label, symbol_head);
    return (bool) (sym != NULL && sym->type == ENTRY_DIRECTIVE_TYPE);
}

int process_directive_first_pass(char *line, int is_label) {
    char *directive_type_str;
    int directive_type, i, address;
    bool is_external, is_action, is_data;
    is_action = FALSE;
    is_data = FALSE;
    is_external = FALSE;

    char extern_label[LABEL_MAX_SIZE];

    char *line_head = line;

    i = 0;
    while (!isspace(*line++))
        i++;
    directive_type_str = strndup(line_head, i);
    directive_type = find_directive_type(directive_type_str);
    if (directive_type == NOT_EXISTS_DIRECTIVE_TYPE) {
        print_error(NOT_EXISTS_DIRECTIVE_ERROR, lines_count);
        return 0;
    }

    if (directive_type == ENTRY_DIRECTIVE_TYPE) {
        return 0; /* Don't handle '.entry' in first pass. */
    } else if (directive_type == EXTERN_DIRECTIVE_TYPE) {
        is_external = TRUE;
        line = strip_label_chars(line, extern_label);
        address = 0;
        add_symbol(extern_label, directive_type, address);
        line = strip_blank_chars(line);
        if (*line != '\0') {
            print_error(SYNTAX_ERROR, lines_count);
        }
        return 0;
    } else {
        is_data = TRUE;
        address = DC;
    }

    if (is_label) {
        add_symbol(label, directive_type, address);
    }

    int num_words = process_data_or_string_line(line);
    return num_words;


}


int process_data_or_string_line(char *line) {
    machine_words *string_words;
    machine_words *number_words;
    machine_words *empty;

    if (*line == '\"') {
        line++;
        string_words = create_string_words(line);
        if (DEBUG) {
            printf("\nProcessing string data store\n");
            print_data_machine_words(string_words);
        }
        add_machine_words(string_words);
    } else if (isdigit(*line)) {

        number_words = create_number_words(line);
        if (DEBUG) {
            printf("\nProcessing numbers data store\n");
            print_data_machine_words(number_words);
        }
        add_machine_words(number_words);
    } else {
        print_error(BAD_COMMAND_ARGUMENT, lines_count);
    }
    return 0;
}


short int is_comment_or_empty(char *line) {
    char c;
    c = *line;
    if (!c || c == COMMENT_CHAR)
        return 1;
    return 0;
}

char *strip_blank_chars(char *line) {
    char c;
    while ((c = *line) && isspace(c))
        line++;
    return line;
}

char *strip_label_chars(char *line, char label_name[LABEL_MAX_SIZE]) {
    char c;
    char *name = label_name;
    char count = 0;

    name = label_name;
    if (!isalpha(*line)) {
        print_error(LABEL_FIRST_CHAR_IS_NOT_ALPHA_ERROR, lines_count);
        label_name = NULL;
        return line;
    }
    *name = *line;
    line++;
    name++;
    count++;
    while ((c = *line) && (isalpha(c) || isdigit(c))) {
        if (count > LABEL_MAX_SIZE) {
            print_error(LABEL_TOO_LONG_ERROR, lines_count);
            label_name = NULL;
            return line;
        }
        *name = *line;
        name++;
        line++;
        count++;
    }
    label_name[count] = '\0';
    return line;
}

char *strip_operand_chars(char *line, char label_name[LABEL_MAX_SIZE]) {
    char c;
    char *name;
    char count = 0;

    name = label_name;
    if (!isalpha(*line) && *line != '#') {
        print_error(BAD_COMMAND_ARGUMENT, lines_count);
        label_name = NULL;
        return line;
    }
    *name = *line;
    line++;
    name++;
    count++;
    while ((c = *line) && !isspace(c) && (isalpha(c) || isdigit(c) || c == '[' || c == ']' || c == '-' || c == '+')) {
        if (count > LABEL_MAX_SIZE) {
            print_error(BAD_COMMAND_ARGUMENT, lines_count);
            label_name = NULL;
            return line;
        }
        *name = *line;
        name++;
        line++;
        count++;
    }
    label_name[count] = '\0';
    return line;
}

char *strip_number(char *line, int *value) {
    char c;
    char *name = (char *) malloc(LABEL_MAX_SIZE);
    char count = 0;
    char *name_head = name;

    int is_minus = 0;

    if (*line == '-') {
        is_minus = 1;
        line++;
    } else if (*line == '+') {
        line++;
    }

    while ((c = *line) && isdigit(c)) {
        *name = c;
        name++;
        line++;
        count++;
    }
    if (!count) {
        print_error(BAD_COMMAND_ARGUMENT, lines_count);
        return NULL;
    } else {
        *value = is_minus ? atoi(name_head) * (-1) : atoi(name_head);
    }

    return line;
}

char *strip_number_or_label(char *line, int *value) {
    char c;
    char *name = (char *) malloc(LABEL_MAX_SIZE);
    char count = 0;
    char *name_head = name;

    int is_minus = 0;

    if (isdigit(*line) || *line == '-' || *line == '+') {
        return strip_number(line, value);
    } else if (isalpha(*line)) {
        line = strip_label_chars(line, name);
        sym_pt macro = search_label(name, symbol_head);
        if (macro == NULL) {
            print_error(LABEL_WITH_NO_DATA_ERROR, lines_count);
        } else {
            *value = macro->value;
        }
        return line;
    }

}

