#include "file_processing.h"
#include "machine_code.h"

addressing_method immediate_addressing_method;
addressing_method direct_addressing_method;
addressing_method permanent_index_addressing_method;
addressing_method direct_register_addressing_method;
addressing_method unknown_addressing_method;


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
    FILE *fp;
    int IC_after_first_pass;
    char *file_name_copy = malloc(sizeof(filename));
    file_name_copy = strcpy(file_name_copy, filename);
    file_name_copy = strcat(file_name_copy, ASSEMBLY_FILE_EXTEN);
    fp = fopen(file_name_copy, "r");
    if (!fp) {
        perror("Error while opening file assembly file");
    }
    IC = 100;
    DC = 0;
    /* Theres a memory leak in the first pass */
    first_pass(fp);
    if (err_count)
        return;
    fclose(fp);
    IC_after_first_pass = IC;
    update_data_addresses(symbol_head, IC_after_first_pass);

    fp = fopen(file_name_copy, "r");
    IC = 100;
    DC = 0;
    second_pass(fp);
    if (err_count)
        return;

    fclose(fp);
    generate_output_files(filename);
    clean_symbol_table(symbol_head);
    clean_externs_table(extern_head);
    clean_code(head_instructions);
    clean_data(head_data);
}


int second_pass(FILE *fp) {
    char line[MAX_CODE_LINE];
    char *line_p;
    line_p = line;
    lines_count = 1;
    while (fgets(line, MAX_CODE_LINE, fp)) {
        line_p = line;

        line_p = strip_blank_chars(line_p);
        if (is_comment_or_empty(line_p)) {
            lines_count++;
            continue;
        }
        process_line_second_pass(line_p);
        lines_count++;

    }

    return 0;
}


void generate_output_files(char *const filename) {
    FILE *fp;
    char *full_name = malloc(sizeof(filename) + sizeof(ASSEMBLY_ENTRIES_FILE_EXTEN));
    full_name = strcpy(full_name, filename);
    full_name = strcat(full_name, ASSEMBLY_ENTRIES_FILE_EXTEN);
    fp = fopen(full_name, "a");
    generate_entries_file(symbol_head, fp);
    fclose(fp);

    full_name = malloc(sizeof(filename) + sizeof(ASSEMBLY_EXTERNAL_FILE_EXTEN));
    full_name = strcpy(full_name, filename);
    full_name = strcat(full_name, ASSEMBLY_EXTERNAL_FILE_EXTEN);
    fp = fopen(full_name, "a");
    generate_externals_file(fp);
    fclose(fp);


    full_name = malloc(sizeof(filename) + sizeof(ASSEMBLY_OBJECTS_FILE_EXTEN));
    full_name = strcpy(full_name, filename);
    full_name = strcat(full_name, ASSEMBLY_OBJECTS_FILE_EXTEN);
    fp = fopen(full_name, "a");
    generate_machine_code_file(fp);
    fclose(fp);
}

void generate_machine_code_file(FILE *fp) {
    machine_words *iterator = head_instructions;
    while (iterator) {
        fprintf(fp, "%d\t%s\n", iterator->address, parse_special_base_4(iterator->value));


        iterator = iterator->next;
    }
    iterator = head_data;
    while (iterator) {
        fprintf(fp, "%d\t%s\n", iterator->address, parse_special_base_4(iterator->value));
        iterator = iterator->next;
    }
}

char *parse_special_base_4(int value) {
    char *parsed = malloc(sizeof(WORD_SIZE / 2) + 1);
    int i;
    unsigned int isolator;
    unsigned int isolated;
    parsed += WORD_SIZE / 2;
    *parsed = '\0';
    parsed--;
    for (i = 0; i < (WORD_SIZE / 2); ++i) {
        isolator = 3u << i * 2;
        isolated = isolator & value;
        isolated >>= i * 2;
        switch (isolated) {
            case 0:
                *parsed = '*';
                parsed--;
                break;
            case 1:
                *parsed = '#';
                parsed--;
                break;
            case 2:
                *parsed = '%';
                parsed--;
                break;
            case 3:
                *parsed = '!';
                parsed--;
                break;
        }
    }
    return ++parsed;
}

void generate_externals_file(FILE *fp) {
    externals_table *iterator = extern_head;
    while (iterator) {
        fprintf(fp, "%s\t%d\n", iterator->external_name, iterator->address);
        iterator = iterator->next;
    }
}

void generate_entries_file(sym_pt head, FILE *fp) {
    if (!head)
        return;

    if (head->type == ENTRY_DIRECTIVE_TYPE) {
        fprintf(fp, "%s\t%d\n", head->label, head->value);
    }

    generate_entries_file(head->left, fp);
    generate_entries_file(head->right, fp);
}

void clean_symbol_table(sym_pt head) {
    if (!head)
        return;
    clean_symbol_table(head->left);
    clean_symbol_table(head->right);
    free(head);
}

void clean_externs_table(externals_table *head) {
    if (!head)
        return;
    clean_externs_table(head->next);
    free(head);
}

void clean_code(machine_words *head) {
    if (!head)
        return;
    clean_code(head->next);
    free(head);
}

void first_pass(FILE *fp) {

    char line[MAX_CODE_LINE];
    char *line_p;
    lines_count = 1;
    line_p = line;
    while (fgets(line, MAX_CODE_LINE, fp)) {
        line_p = line;
        /*Printing the addresses of pointers*/
        /*printf("%p\n", (void *)line_p);
        printf("%p\n", (void *)line);*/

        line_p = strip_blank_chars(line_p);
        if (is_comment_or_empty(line_p)) {
            lines_count++;
            continue;
        }
        process_line_first_pass(line_p);
        lines_count++;
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


        /* Dealing with Label */
        is_label = find_label(line, TRUE);
        if (is_label) {
            line = go_to_next_field(line);
            if (line == NULL) {
                err_count++;
                print_error(LABEL_WITH_NO_DATA_ERROR, lines_count);
                return 0;
            }
        }


        /* Looking for directive... */
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
    int is_label;
    char *directive_type_str;
    int i = 0;
    int directive_type;
    char *line_head;
    opcode_p opcode;

    /******** Dealing with Label *******************/
    is_label = find_label(line, FALSE);
    if (is_label) {
        line = go_to_next_field(line);
    }

    if (*line == '.') {
        line++;
    }

    line_head = line;

    while (!isspace(*line++)) {
        i++;
    }
    directive_type_str = strndup_local(line_head, i);
    directive_type = find_directive_type(directive_type_str);


    switch (directive_type) {
        case ENTRY_DIRECTIVE_TYPE:
            process_entry_line(line);
            return;
        case MACRO_DIRECTIVE_TYPE:
            return;
        case DATA_DIRECTIVE_TYPE :
            return;
        case STRING_DIRECTIVE_TYPE:
            return;
        case EXTERN_DIRECTIVE_TYPE:
            return;
    }


    opcode = get_opcode_node(directive_type_str);
    process_instruction_second_pass(line, *opcode);


}

void process_instruction_second_pass(char *line, Opcode command) {

    char operands[2][LABEL_MAX_SIZE];
    machine_words *first_word;
    machine_words *current_word;
    machine_word_instruction *empty = (machine_word_instruction *) malloc(sizeof(machine_word_instruction));
    int i;
    int num_of_operands;
    int num_of_total_words = get_number_of_instruction_words(line, empty, command);

    if (num_of_total_words == 1)
        return;

    num_of_operands = strip_operands(line, operands);

    first_word = get_machine_word_by_address(IC);

    if (DEBUG) {
        printf("Address: %d, Value: %s\n", first_word->address, parse_word_string_represntation(*first_word));
    }

    if (!first_word)
        printf("Error while finding word on second phase!");

    current_word = first_word->next;

    if (num_of_operands == 2 &&
        get_operand_addressing_method(operands[0]).method == direct_register_addressing_method.method &&
        get_operand_addressing_method(operands[1]).method == direct_register_addressing_method.method) {
        set_register_field_in_word(current_word, operands[0], SRC_REGISTER);
        set_register_field_in_word(current_word, operands[1], DEST_REGISTER);
        if (DEBUG) {
            printf("Address: %d, Value: %s\n", current_word->address, parse_word_string_represntation(*current_word));
        }
    } else {
        for (i = 0; i < num_of_operands; ++i) {
            current_word = process_operand(operands[i], current_word, (i == 0) ? SRC_REGISTER : DEST_REGISTER);
        }
    }


    IC += num_of_total_words;

}

void set_register_field_in_word(machine_words *word, char *string, int src_or_dest) {
    unsigned int value = is_register(string);
    if (src_or_dest == SRC_REGISTER) {
        value <<= 5;

    } else if (src_or_dest == DEST_REGISTER) {
        value <<= 2;
    }
    word->value |= value;
}

machine_words *process_operand(char operand[50], machine_words *current_word, int operand_type) {
    int immediate_value;
    int address;
    unsigned int register_num;
    char *operand_head;
    char *operand_address;
    int i;
    sym_pt label;

    addressing_methods method = get_operand_addressing_method(operand).method;
    switch (method) {
        case IMMEDIATE:
            strip_number_or_label(++operand, &immediate_value);
            immediate_value <<= 2;
            current_word->value = immediate_value;
            return current_word->next;
        case DIRECT:
            label = search_label(operand, symbol_head);
            if (!label) {
                print_error(LABEL_NOT_FOUND, lines_count);
                err_count++;
                return current_word->next;
            }
            if (label->type == EXTERN_DIRECTIVE_TYPE) {
                add_to_externals(label->label, current_word->address);
            }
            current_word->value = (label->value) << 2;

            /* set A R E field */
            current_word->value = (label->type == EXTERN_DIRECTIVE_TYPE) ? (current_word->value) | 1u :
                                  (current_word->value) | 2u;
            return current_word->next;
        case PERMANENT_INDEX:
            operand_head = operand;
            i = 0;
            while (*operand != '[') {
                i++;
                operand++;
            }
            operand_address = strndup_local(operand_head, i);
            label = search_label(operand_address, symbol_head);
            address = label->value;
            address <<= 2;
            if (label->type == EXTERN_DIRECTIVE_TYPE)
                address |= 1;
            else
                address |= 2;

            current_word->value = address;
            current_word = current_word->next;
            operand++;

            strip_number_or_label(operand, &address);
            address <<= 2;
            current_word->value = address;
            return current_word->next;

        case DIRECT_REGISTER:
            register_num = is_register(operand);
            if (operand_type == SRC_REGISTER)
                register_num <<= 5;
            else if (operand_type == DEST_REGISTER)
                register_num <<= 2;
            else
                printf("Unrecognized operand type at line: %d", lines_count);

            current_word->value = register_num;
            return current_word->next;
        default:
            printf("Unable to create operands extra words for operand: %s", operand);
            break;

    }
    return NULL;
}


void process_entry_line(char *line) {
    char entry_name[LABEL_MAX_SIZE];
    sym_pt label;

    line = strip_blank_chars(line);

    strip_label_chars(line, entry_name);

    label = search_label(entry_name, symbol_head);

    if (!label) {
        print_error(ENTRY_NOT_EXIST, lines_count);
        return;
    }

    label->type = ENTRY_DIRECTIVE_TYPE;

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

    if (!is_already_exists_label(name, 0)) {
        sym_pt sym = create_symbol_node(name, value, find_directive_type("define"));
        insert_symbol_to_tree(sym);
    }


    return 0;
}

int process_instruction_first_pass(char *line, int is_label) {
    machine_word_instruction *first_word;
    opcode_p opcode;
    machine_words *word;

    char instruction[MAX_OPCODE_BITS];
    int L;
    int i;

    if (DEBUG)
        printf("Processing Phase 1 instruction: \n\t%s\n", line);

    if (is_label) {
        add_symbol(label, find_directive_type("code"), IC);

    }
    if (!isalpha(*line)) {
        print_error(INSTRUCTION_NOT_FOUND, lines_count);
        return 0;
    }
    line = strip_label_chars(line, instruction);

    opcode = get_opcode_node(instruction);

    if (!opcode) {
        print_error(INSTRUCTION_NOT_FOUND, lines_count);
        return 0;
    }
    line = strip_blank_chars(line);

    first_word = (machine_word_instruction *) malloc(sizeof(machine_word_instruction));

    L = get_number_of_instruction_words(line, first_word, *opcode);

    word = parse_word_as_unsigned_int(*first_word);
    word->address = IC++;
    word->desc = line;
    add_machine_words(word, CODE_DIRECTIVE_TYPE);
    for (i = 1; i < L; ++i) {
        word = create_empty_word();
        word->address = IC++;
        word->desc = line;
        add_machine_words(word, CODE_DIRECTIVE_TYPE);
    }

    return 0;
}

machine_words *create_empty_word() {
    return (machine_words *) malloc(sizeof(machine_words));
}

bool find_label(char *line, bool print_errors) {
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
            if (!isdigit(label[c]) && !isalpha(label[c])) {
                print_error(LABEL_CONTAINS_INVALID_CHARACTER_ERROR, lines_count);
                return FALSE;
            }
        }
        if (!isalpha(*label)) { /* label must start with an alpha char */
            print_error(LABEL_FIRST_CHAR_IS_NOT_ALPHA_ERROR, lines_count);
            return FALSE;
        }
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
    int directive_type, i, address, num_words;

    char extern_label[LABEL_MAX_SIZE];

    char *line_head = line;

    i = 0;
    while (!isspace(*line++))
        i++;
    directive_type_str = strndup_local(line_head, i);
    directive_type = find_directive_type(directive_type_str);
    if (directive_type == NOT_EXISTS_DIRECTIVE_TYPE) {
        print_error(NOT_EXISTS_DIRECTIVE_ERROR, lines_count);
        return 0;
    }

    if (directive_type == ENTRY_DIRECTIVE_TYPE) {
        return 0; /* Don't handle '.entry' in first pass. */
    } else if (directive_type == EXTERN_DIRECTIVE_TYPE) {
        line = strip_label_chars(line, extern_label);
        address = 0;
        add_symbol(extern_label, directive_type, address);
        line = strip_blank_chars(line);
        if (*line != '\0') {
            print_error(SYNTAX_ERROR, lines_count);
        }
        return 0;
    } else {
        address = DC;
    }


    if (is_label) {
        add_symbol(label, directive_type, DC);
    }

    num_words = process_data_or_string_line(line);
    return num_words;


}


int process_data_or_string_line(char *line) {

    machine_words *string_words;
    machine_words *number_words;

    if (*line == '\"') {
        line++;
        string_words = create_string_words(line);
        if (DEBUG) {
            printf("\nProcessing string data store\n");
            print_data_machine_words(string_words);
        }
        add_machine_words(string_words, STRING_DIRECTIVE_TYPE);
        return get_number_of_words(string_words);
    } else if (isdigit(*line)) {

        number_words = create_number_words(line);
        if (DEBUG) {
            printf("\nProcessing numbers data store\n");
            print_data_machine_words(number_words);
        }
        add_machine_words(number_words, DATA_DIRECTIVE_TYPE);
        return get_number_of_words(number_words);
    } else {
        print_error(BAD_COMMAND_ARGUMENT, lines_count);
        return 0;
    }
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
    int count = 0;

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
    int count = 0;

    name = label_name;

    if (!*line)
        return NULL;

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
    sym_pt label;
    char *name = (char *) malloc(LABEL_MAX_SIZE);

    if (isdigit(*line) || *line == '-' || *line == '+') {
        return strip_number(line, value);
    } else if (isalpha(*line)) {
        line = strip_label_chars(line, name);
        label = search_label(name, symbol_head);
        if (label == NULL) {
            print_error(LABEL_NOT_FOUND, lines_count);
        } else {
            *value = label->value;
        }
        return line;
    }
    return line;
}

char *strndup_local(const char *s, size_t n) {
    char *result;
    size_t len = strnlen_local(s, n);

    result = (char *) malloc(len + 1);
    if (!result)
        return 0;

    result[len] = '\0';
    return (char *) memcpy(result, s, len);
}


char *strdup_local(char *src) {
    char *str;
    char *p;
    int len = 0;

    while (src[len])
        len++;
    str = malloc(len + 1);
    p = str;
    while (*src)
        *p++ = *src++;
    *p = '\0';
    return str;
}

size_t strlen_local(const char *str) {
    const char *s;

    for (s = str; *s; ++s);
    return (s - str);
}

size_t strnlen_local(const char *s, size_t max_len) {
    size_t i = 0;
    for (; (i < max_len) && s[i]; ++i);
    return i;
}