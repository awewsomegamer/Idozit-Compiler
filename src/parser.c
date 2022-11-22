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
tree_code_t* addition();

tree_code_t* term() {
        tree_code_t* ret = create_empty(0, 0);
        printf("TERM\n");

        if (accept(T_LPAREN)) {
                printf("LPAREN\n");
                ret = addition();
                expect(T_RPAREN);

                return ret;
        } else if (accept(T_NUMBER)) {
                // Number
                printf("NUMBER\n");
                
                ret->type = T_NUMBER;
                ret->value = last_token->value;
                
                return ret;
        } else if (accept(T_VAR)) {
                // Variable
                printf("VARIABLE\n");
                
                ret->type = T_VAR;
                ret->value = last_token->value;
                
                return ret;
        } else if (accept(T_IDENT)) {
                // Function

                printf("FUNCTION\n");

                int degree = 1;
                if (accept(T_NUMBER))
                        degree = last_token->value;
                
                expect(T_LPAREN);
                ret = addition();
                expect(T_RPAREN);
                ret->value = degree;
                ret->type = T_IDENT;

                return ret;
        }
}

tree_code_t* multiplication() {
        int cont_loop = 1;
        tree_code_t *left, *right;

        left = term();
        printf("MULTIPLICATION / DIVISION\n");
        cont_loop = accept(T_MUL);
        cont_loop = accept(T_DIV) << 1;

        if (cont_loop) 
                right = multiplication();

        tree_code_t* tree = create_node(((cont_loop & 1) ? T_MUL : T_DIV), 0, left, right);

        return tree;
}

tree_code_t* addition() {
        int cont_loop = 1;
        tree_code_t *left, *right;
        left = multiplication();
        printf("ADDITION / SUBTRACTION\n");
        
        cont_loop = accept(T_ADD);
        cont_loop = accept(T_SUB) << 1;

        if (cont_loop) 
                right = addition();

        tree_code_t* tree = create_node(((cont_loop & 1) ? T_ADD : T_SUB), 0, left, right);

        return tree;
}

// Implement settings hooks
//      -> Error pass through
tree_code_t* build_tree() {
        current_token = malloc(sizeof(token_t));
        lex(current_token);

        return addition();
}

double evaluate_tree(tree_code_t* head) {
        double left = 0, right = 0;
        if (head->branches.left != NULL) left = evaluate_tree(head->branches.left);
        if (head->branches.right != NULL) right = evaluate_tree(head->branches.right);

        switch (head->type) {
        case T_ADD: return left + right;
        case T_SUB: return left - right;
        case T_MUL: return left * right;
        case T_DIV: return left / right;
        }
}

