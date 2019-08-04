#include "file_processing.h"


bool is_already_exists_label(char label[50]);

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
    char actions[MAX_CODE_LINE][WORD_SIZE];
    FILE *fp = fopen("example.asm", "r");
    if (!fp) {
        perror("Error while opening file");
    }

    first_pass(fp);
    if (err_count)
        return;
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

void second_pass(FILE *pFile) {

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
                return 0;
            }
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
            return process_instruction(line, is_label);
        }
    }
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

    if (*line_pt != "\n") {
        print_error(SYNTAX_ERROR, lines_count);
        return 0;
    }

    if (!is_already_exists_label(name)) {
        sym_pt sym = create_symbol_node(name, value, find_directive_type("define"));
        insert_symbol_to_tree(sym);
    }


    return 0;
}

int process_instruction(char *line, int label) {
    return 0;
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
    sym_pt new_symbol;
    char *directive_type_str;
    int directive_type, i, address;
    bool is_external, is_action, is_data;
    is_action = FALSE;
    is_data = FALSE;
    is_external = FALSE;

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
        strip_label_chars(line, label);
        address = 0;
    } else {
        is_data = TRUE;
        address = DC;
    }

    if (is_label) {
        if (!is_already_exists_label(label)) {
            new_symbol = create_symbol_node(label, address, directive_type);
            insert_symbol_to_tree(new_symbol);
        }
    }

    if (is_data) {

    }

}

bool is_already_exists_label(char label[50]) {
    if (is_keyword(label)) {
        print_error(LABEL_IS_KEYWORD_ERROR, lines_count);
        return TRUE;
    }
    sym_pt already_exist_sym = search_label(label, symbol_head);
    if (already_exist_sym)
        if (already_exist_sym->type == ENTRY_DIRECTIVE_TYPE) {
            print_error(LABEL_IS_ALREADY_ENTRY, lines_count);
            return TRUE;
        } else if (already_exist_sym->type == EXTERN_DIRECTIVE_TYPE) {
            print_error(LABEL_IS_ALREADY_EXTERN, lines_count);
            return TRUE;
        } else if (already_exist_sym->type == MACRO_DIRECTIVE_TYPE) {
            print_error(LABEL_IS_ALREADY_MACRO, lines_count);
            return TRUE;
        }
    return FALSE;
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
    char *name;
    char count = 0;

    name = label_name;
    if (!isalpha(*line)) {
        print_error(LABEL_FIRST_CHAR_IS_NOT_ALPHA_ERROR, lines_count);
        return line;
    }
    *name = *line;
    line++;
    name++;
    while ((c = *line) && (isalpha(c) || isdigit(c))) {
        if (count > LABEL_MAX_SIZE) {
            print_error(LABEL_TOO_LONG_ERROR, lines_count);
            return line;
        }
        *name = *line;
        name++;
        line++;
        count++;
    }
    return line;
}

char *strip_number(char *line, int *value) {
    char c;
    char *name = (char *) malloc(LABEL_MAX_SIZE);
    char count = 0;
    char *name_head = name;

    while ((c = *line) && isdigit(c)) {
        *name = c;
        name++;
        line++;
        count++;
    }
    if (!count) {
        print_error(BAD_COMMAND_ARGUMENT, lines_count);
    } else {
        *value = atoi(name_head);
    }
    return line;
}
