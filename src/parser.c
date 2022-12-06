#include <parser.h>
#include <lexer.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <messages.h>

token_t *current_token = NULL;
token_t *last_token = NULL;

// Functions to create nodes
tree_code_t* create_node(int type, double value, tree_code_t *left, tree_code_t *right)
{
        tree_code_t *node = malloc(sizeof(tree_code_t));

        node->type = type;
        node->value = value;
        node->left = left;
        node->right = right;

        return node;
}

#define create_empty(type, value) create_node(type, value, NULL, NULL)

/* uint8_t accept(uint8_t type) :
 * Is the current token's type == the given type?
 *
 * Yes: Move the current token to the last token, move to the next token, return 1
 * No: Return 0
 */
uint8_t accept(uint8_t type)
{
        if (current_token->type == type) {
                last_token->type = current_token->type;
                last_token->value = current_token->value;

                lex(current_token);
                return 1;
        }

        return 0;
}

/* void expect(uint8_t type) :
 * Expect the token type, type. If the accept
 * function returns 1, move on, otherwise something
 * is wrong with the program.
 */
void expect(uint8_t type)
{
        if (accept(type))
                return;

        message(MESSAGE_FATAL, "Expected token type %d (%s) instead of %d (%s)\n", type, TOKEN_NAMES[type], current_token->type, TOKEN_NAMES[current_token->type]);
}

// Recursive Descent Parser:
tree_code_t *addition();
tree_code_t *term();
double evaluate_tree(tree_code_t *head);

tree_code_t *exponent(tree_code_t *ret)
{
        tree_code_t* exponent_ret = create_empty(T_EXPONENT, 0);
        exponent_ret->left = ret;
        exponent_ret->right = term();

        ret->parent = exponent_ret;
        exponent_ret->right->parent = exponent_ret;

        message(MESSAGE_DEBUG, "EXPONENT\n");
        
        return exponent_ret;
}

tree_code_t* find_node(tree_code_t* tree, int type, int value) {
        if (tree != NULL && tree->type == type && tree->value == value)
                return tree;

        if (tree->left != NULL) return find_node(tree->left, type, value);
        if (tree->right != NULL) return find_node(tree->right, type, value);

        return NULL;
}

void apply_function(int function, int degree, int respect_to, tree_code_t* tree) {
        switch (function) {
        case T_FUNC_DERIVATIVE:
                // 2 * x = 2
                // 2 * 2x = 4
                // 2 * x^2 = 4 * x
                // 2 * x^3 = 6 * x^2
                // What we want to do is go down the ret tree
                // and apply this rule, the variable gets toned
                // down, and if there is a multiplication or divisio
                // in common, then it will be applied.

                // We basically want to look at every variable
                // And apply the derivative formula

                tree_code_t* var, *parent, *new_node;
                
                // While you can find the respected variable:
                // Get the pointer to the variable (done)
                // Go to its parent and do the above if statement on it
                // If it passes the if statement then derive it
                // Finally collapse back up to caller
                // Figure out a way to remove random constants
                // which have no connection to the respected
                // variable. "x + 2" 2 would be removed


                // do {
                        var = find_node(tree, T_VAR, respect_to);
                        parent = var->parent;
                        new_node = create_empty(0, 0);

                        // Check if parent node is a ^
                        // I don't yet know how to evaluate
                        // derivateives of variables which use
                        // another variable as their power
                        // so for now all it will be is just numbers


                        if (var->parent->type == T_EXPONENT) {
                                int value = evaluate_tree(parent->right);
                                
                                parent->type = T_MUL;
                                parent->left = new_node;
                                parent->right = create_empty(T_INT, value);

                                new_node->left = create_empty(T_VAR, var->value);
                                new_node->right = create_empty(T_INT, value - 1);
                        } else if (var->parent->type == T_MUL) {

                        } else {

                        }


                // } while (var != NULL);
                        



                break;
        }
}

tree_code_t *term()
{
        tree_code_t *ret = create_empty(0, 0);

        if (accept(T_LPAREN)) {
                // ( EXPRESSION )

                message(MESSAGE_DEBUG, "LPAREN\n");
                if (current_token->type != T_RPAREN)
                        ret = addition();
                expect(T_RPAREN);

                if (accept(T_EXPONENT)) return exponent(ret);

                return ret;
        } else if (accept(T_NUMBER) || accept(T_INT)) {
                // Number

                ret->type = last_token->type;
                ret->value = last_token->value;
                message(MESSAGE_DEBUG, "NUMBER %f\n", ret->value);

                if (accept(T_EXPONENT)) return exponent(ret);

                return ret;
        } else if (accept(T_VAR)) {
                // Variable

                message(MESSAGE_DEBUG, "VARIABLE\n");
                
                ret->type = T_VAR;
                ret->value = last_token->value;
                
                if (accept(T_EXPONENT)) return exponent(ret);

                return ret;
        } else if (accept(T_IDENT)) {
                // Function

                message(MESSAGE_DEBUG, "FUNCTION\n");

                int func = last_token->value;

                int degree = 1;
                if (accept(T_INT))
                        degree = last_token->value;
                
                expect(T_VAR);
                int respect_to = last_token->value;

                expect(T_LPAREN);
                ret = addition();
                expect(T_RPAREN);
                
                apply_function(func, degree, respect_to, ret); 

                if (accept(T_EXPONENT)) return exponent(ret);

                return ret;
        }
}

tree_code_t *multiplication()
{
        int cont_loop = 0; // 0: Return left, 1: Continue multiplication, 2: Continue division
        tree_code_t *left, *right;

        left = term();
        
        message(MESSAGE_DEBUG, "MULTIPLICATION / DIVISION\n");

        cont_loop = accept(T_MUL);
        cont_loop += accept(T_DIV) << 1;

        if (!cont_loop || current_token->type == T_EOF || last_token->type == T_EOF)
                return left;

        right = multiplication();

        tree_code_t *tree = create_node(((cont_loop & 1) ? T_MUL : T_DIV), 0, left, right);
        
        left->parent = tree;
        right->parent = tree;

        return tree;
}

tree_code_t *addition()
{
        int cont_loop = 0; // 0: Return left, 1: Continue addition, 2: Continue subtraction
        tree_code_t *left, *right;

        left = multiplication();

        message(MESSAGE_DEBUG, "ADDITION / SUBTRACTION\n");
        
        cont_loop = accept(T_ADD);
        cont_loop += accept(T_SUB) << 1;

        if (!cont_loop || current_token->type == T_EOF || last_token->type == T_EOF) 
                return left;

        right = addition();

        tree_code_t *tree = create_node(((cont_loop & 1) ? T_ADD : T_SUB), 0, left, right);
        
        left->parent = tree;
        right->parent = tree;

        return tree;
}

/* tree_code_t *build_tree() :
 * This function will build the AST
 * which can be used later.
 */
tree_code_t *build_tree()
{
        current_token = malloc(sizeof(token_t));
        last_token = malloc(sizeof(token_t));
        lex(current_token);

        tree_code_t* tree = addition();

        free(current_token);
        free(last_token);

        return tree;
}

double evaluate_tree(tree_code_t *head)
{
        double left = 0, right = 0;

        if (head->left != NULL) left = evaluate_tree(head->left);
        if (head->right != NULL) right = evaluate_tree(head->right);

        switch (head->type) {
        case T_ADD: return left + right;
        case T_SUB: return left - right;
        case T_MUL: return left * right;
        case T_DIV: return left / right;
        case T_EXPONENT: {
                double result = 1;

                for (double i = 0; i < right; i++)
                        result *= left;

                return result;
        }
        case T_INT:
        case T_NUMBER: return head->value;
        }
}