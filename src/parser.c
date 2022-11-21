#include <parser.h>
#include <lexer.h>

tree_code_t* current_node = NULL;
token_t* current_token = NULL;
token_t* last_token = NULL;

tree_code_t* create_node(int type, double value, tree_code_t* left, tree_code_t* right) {
        tree_code_t* node = malloc(sizeof(tree_code_t));

        node->type = type;
        node->value = value;
        node->branches.left = left;
        node->branches.right = right;

        return node;
}

#define create_empty(type, value) create_node(type, value, NULL, NULL)

uint8_t accept(uint8_t type) {
        if (current_token->type == type) {
                last_token = current_token;
                lex(current_token);
                return 1;
        }

        return 0;
}

void expect(uint8_t type) {
        if (accept(type))
                return;

        // Big error
}

// Parse body:

void addition();

void term() {
        if (accept(T_LPAREN)) {
                printf("LPAREN\n");
                addition();
                expect(T_RPAREN);
        } else if (accept(T_NUMBER)) {
                // Number
                // current_node->branches[current_node->used++] = create_empty(T_NUMBER, last_token->value);
                printf("NUMBER\n");
        } else if (accept(T_VAR)) {
                // Variable
                // current_node->branches[current_node->used++] = create_empty(T_VAR, last_token->value);
                printf("VARIABLE\n");
        } else if (accept(T_IDENT)) {
                // Function

        }
}

void multiplication() {
        do {
                printf("MUL\n");
                term();
        } while (accept(T_MUL) || accept(T_DIV));
}

void addition() {
        do {
                printf("ADD\n");
                multiplication();
        } while (accept(T_ADD) || accept(T_SUB));
}

tree_code_t* build_tree() {
        tree_code_t* head = create_empty(0, 0);
        current_node = head;

        addition();

        return head;
}
