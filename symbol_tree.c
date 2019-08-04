#include "symbol_tree.h"

sym_pt symbol_head = NULL;

sym_pt create_symbol_node(char *label, int address, int type)
{
    sym_pt new_symbol;
    new_symbol = (sym_pt)(malloc(sizeof(Symbol)));
    new_symbol->label = strdup(label);
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
        return insert_symbol(new_symbol, &((*cur_node)->right));
    } else if (strcmp_result < 0) {
        return insert_symbol(new_symbol, &((*cur_node)->left));
    } else {
        return;
    }
}

sym_pt search_label(char *label, sym_pt node) {
    int strcmp_result;
    if (node != 0) {
        if ((strcmp_result = strcmp(label, node->label)) == 0) {
            return node;
        } else if (strcmp_result > 1) {
            return search_label(label, node->right);
        } else {
            return search_label(label, node->left);
        }
    } else
        return NULL;
}

void print_symbol_table_recursion(sym_pt head)
{
    if(head)
    {
        printf("%-25s%-20d%-10d\n", head->label, head->value,head->type);
        print_symbol_table_recursion(head->left);
        print_symbol_table_recursion(head->right);
    }

}

void print_symbol_table()
{
    printf("%-25s%-20s%-10s\n", "Name", "Value", "Type");
    print_symbol_table_recursion(symbol_head);

}