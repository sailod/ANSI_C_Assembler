#include "machine_code.h"
#include "file_processing.h"
#include "symbol_tree.h"

sym_pt symbol_head = NULL;


sym_pt create_symbol_node(char *label, int address, int type) {
    sym_pt new_symbol;
    new_symbol = (sym_pt) (malloc(sizeof(Symbol)));
    new_symbol->label = strdup_local(label);
    new_symbol->value = address;
    new_symbol->type = type;
    return new_symbol;
}

void insert_symbol_to_tree(sym_pt new_symbol) {
    insert_symbol(new_symbol, &symbol_head);
}

void insert_symbol(sym_pt new_symbol, sym_pt *cur_node) {
    int strcmp_result;
    if (!*cur_node) {
        *cur_node = new_symbol;
        /* initialize the children to null */
        (*cur_node)->left = 0;
        (*cur_node)->right = 0;
    } else if ((strcmp_result = strcmp(new_symbol->label, (*cur_node)->label)) > 0) {
        insert_symbol(new_symbol, &((*cur_node)->right));
    } else if (strcmp_result < 0) {
        insert_symbol(new_symbol, &((*cur_node)->left));
    } else {
        return;
    }
}

sym_pt search_label(char *label, sym_pt node) {
    int strcmp_result;
    sym_pt left;
    sym_pt right;
    if (node != NULL) {
        if ((strcmp_result = strcmp(label, node->label)) == 0) {
            return node;
        }

        left = search_label(label, node->left);
        right = search_label(label, node->right);
        return left != NULL ? left : right;
    } else
        return NULL;
}

void print_symbol_table_recursion(sym_pt head) {
    if (head) {
        printf("%-25s%-20d%-10d\n", head->label, head->value, head->type);
        print_symbol_table_recursion(head->left);
        print_symbol_table_recursion(head->right);
    }

}

void print_symbol_table() {
    printf("%-25s%-20s%-10s\n", "Name", "Value", "Type");
    print_symbol_table_recursion(symbol_head);

}

void add_symbol(char *label, int directive_type, int address) {
    sym_pt new_symbol;
    if (!is_already_exists_label(label, 0)) {
        new_symbol = create_symbol_node(label, address, directive_type);
        insert_symbol_to_tree(new_symbol);
    }
}

bool is_already_exists_label(char label[50], bool print_errors) {
    sym_pt already_exist_sym;
    if (is_keyword(label)) {
        if (print_errors) {
            print_error(LABEL_IS_KEYWORD_ERROR, lines_count);
        }
        return TRUE;
    }
    already_exist_sym = search_label(label, symbol_head);
    if (already_exist_sym) {
        if (already_exist_sym->type == ENTRY_DIRECTIVE_TYPE) {
            if (print_errors) {
                print_error(LABEL_IS_ALREADY_ENTRY, lines_count);
            }
            return TRUE;
        } else if (already_exist_sym->type == EXTERN_DIRECTIVE_TYPE) {
            if (print_errors) {
                print_error(LABEL_IS_ALREADY_EXTERN, lines_count);
            }
            return TRUE;
        } else if (already_exist_sym->type == MACRO_DIRECTIVE_TYPE) {
            if (print_errors) {
                print_error(LABEL_IS_ALREADY_MACRO, lines_count);
            }
            return TRUE;
        }
    } else
        return FALSE;
    return FALSE;
}

void update_data_label_addresses(sym_pt head, int last_IC) {
    if (!head)
        return;
    if (head->type == DATA_DIRECTIVE_TYPE)
        head->value += last_IC + 100;
    update_data_label_addresses(head->left, last_IC);
    update_data_label_addresses(head->right, last_IC);

}

