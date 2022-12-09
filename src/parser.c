#include <parser.h>
#include <lexer.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <messages.h>

token_t *current_token = NULL;
token_t *last_token = NULL;

// Functions to manipulate nodes
tree_code_t* create_node(int type, double value, uint64_t parser_mark, tree_code_t *left, tree_code_t *right)
{
        tree_code_t *node = malloc(sizeof(tree_code_t));

        node->type = type;
        node->value = value;
        node->left = left;
        node->right = right;
        node->parser_mark = parser_mark;

        return node;
}

#define create_empty(type, value) create_node(type, value, 0, NULL, NULL)

// Recursively free tree
void free_tree(tree_code_t* tree) {
        if (tree->left != NULL) free_tree(tree->left);
        if (tree->right != NULL) free_tree(tree->right);

        free(tree);
}


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

tree_code_t* find_node(tree_code_t* tree, int type, double value, uint64_t parser_mark) {
        if (tree != NULL && tree->type == type && tree->value == value && tree->parser_mark == parser_mark)
                return tree;

        tree_code_t *branch = NULL;

        if (tree->left != NULL) branch = find_node(tree->left, type, value, parser_mark);
        if (branch != NULL) return branch;
        
        if (tree->right != NULL) branch = find_node(tree->right, type, value, parser_mark);
        if (branch != NULL) return branch;

        return NULL;
}

void tree_set_value(tree_code_t* tree, uint64_t mark, double value) {
        if (tree->parser_mark != mark) {
                tree->value = value;

                if (tree->left != NULL) tree_set_value(tree->left, mark, value);
                if (tree->right != NULL) tree_set_value(tree->right, mark, value);
        }
}

void parent_branch(tree_code_t* tree, uint64_t mark, tree_code_t* parent) {
        if (tree != NULL && tree->parser_mark != mark) {
                if (tree->left != NULL) parent_branch(tree->left, mark, parent);
                if (tree->right != NULL) parent_branch(tree->right, mark, parent);

                tree_code_t* filled_branch = malloc(sizeof(tree_code_t));
                memcpy(filled_branch, tree, sizeof(tree_code_t));

                tree_code_t* child = create_node(tree->type, tree->value, tree->parser_mark, tree->left, tree->right);
                tree->type = parent->type;
                tree->value = parent->value;
                tree->parser_mark = parent->parser_mark;

                if (parent->left == NULL) {
                        tree->left = filled_branch;
                        tree->right = parent->right;
                } else if (parent->right == NULL) {
                        tree->left = parent->left;
                        tree->right = filled_branch;
                }
        }
}

void apply_function(int function, int degree, int respect_to, tree_code_t* tree) {
        tree_code_t* var, *parent, *new_node;
        
        switch (function) {
        case T_FUNC_DERIVATIVE: {
                do {
                        var = find_node(tree, T_VAR, respect_to, 0);
                        
                        if (var == NULL) break;

                        parent = var->parent;
                        new_node = create_empty(0, 0);

                        if (parent->type == T_EXPONENT) {
                                int value = evaluate_tree(parent->right);

                                if (value == 0) {
                                        parent->type = T_INT;
                                        parent->value = 0;

                                        free_tree(parent->left);
                                        free_tree(parent->right);

                                        parent->left = NULL;
                                        parent->right = NULL;

                                        continue;
                                }

                                parent->parser_mark = 2;

                                parent->type = T_MUL;
                                parent->left = new_node;
                                parent->right = create_empty(T_INT, value);

                                new_node->type = T_EXPONENT;
                                new_node->left = create_empty(T_VAR, var->value);
                                new_node->left->parser_mark = 1;
                                new_node->right = create_empty(T_INT, value - 1);
                        } else if (parent->type == T_MUL) {
                                parent->parser_mark = 2;
                                
                                if (var == parent->left) {
                                        parent->type = parent->right->type;
                                        parent->value = parent->right->value;
                                } else if (var == parent->right) {
                                        parent->type = parent->left->type;
                                        parent->value = parent->left->value;
                                }

                                free_tree(parent->left);
                                free_tree(parent->right);

                                parent->left = NULL;
                                parent->right = NULL;
                        } else {
                                var->parser_mark = 2;
                                var->type = T_INT;
                                var->value = 1;
                        }

                        // Mark top most branch of the variable 2
                        while (parent->parent != NULL && (parent->parent->type == T_MUL || parent->parent->type == T_DIV))
                                parent = parent->parent;

                        parent->parser_mark = 2;
                } while (var != NULL);

                // Zero Code
                tree_set_value(tree, 2, 0);

                break;
        }

        case T_FUNC_INTEGRAL: {
                /* While we can find variables:
                 *      -> Go to them and:
                 *              -> If its in an exponent:
                 *                      -> Add 1 to the power
                 *                      -> Multiply the exponent by 1 / power + 1
                 *              -> If it is in a multiplication:
                 *                      -> 3x = 3/2 * x^2
                 *                      -> 2x = x^2
                 *                      -> x  = 1/2 * x^2
                 * 
                 *      -> For numbers with no variables attached:
                 *              -> Put them into a tree where they are
                 *                 multiplied by the variable.
                 */

                // Handle respected variable
                do {
                        var = find_node(tree, T_VAR, respect_to, 0);
                        
                        if (var == NULL) break;

                        parent = var->parent;
                        new_node = create_empty(0, 0);
                        
                        int nm_left = 0;
                        if (parent->type == T_EXPONENT) {
                                int value = evaluate_tree(parent->right) + 1;
                                
                                new_node = create_empty(T_EXPONENT, 0);
                                new_node->left = create_empty(T_VAR, respect_to);
                                new_node->left->parser_mark = 1;
                                new_node->right = create_empty(T_INT, value);

                                parent->type = T_DIV;
                                parent->left = new_node;
                                parent->right = create_empty(T_INT, value);
                                parent->parser_mark = 2;
                        } else if (parent->type == T_MUL && ((nm_left = (parent->left->type == T_INT || parent->left->type == T_NUMBER)) || (parent->right->type == T_INT || parent->right->type == T_NUMBER))) {
                                parent->parser_mark = 2;

                                if (nm_left == 0) {
                                        parent->right->value = evaluate_tree(parent->right) / 2;


                                        if (parent->right->value == 1) {
                                                free_tree(parent->right);
                                                parent->right = NULL;
                                                parent->value = var->value;
                                                var = parent;
                                                goto MUL_GEN;
                                        }

                                        parent->right->type = T_NUMBER;
                                        printf("%f\n", parent->right->value);
                                } else {
                                        parent->left->value = evaluate_tree(parent->left) / 2;

                                        if (parent->left->value == 1) {
                                                free_tree(parent->left);
                                                parent->left = NULL;
                                                parent->value = var->value;
                                                var = parent;
                                                goto MUL_GEN;
                                        }

                                        parent->left->type = T_NUMBER;
                                        printf("%f\n", parent->left->value);
                                }

                                goto MUL_GEN;
                        } else {
                                MUL_GEN:

                                var->type = T_MUL;
                                var->left = create_empty(T_VAR, var->value);
                                var->right = var->left;
                                var->left->parser_mark = 1;
                                var->parser_mark = 2;
                        }
                } while (var != NULL);

                // Now we need to set all numbers, integers, variables to be multiplied by X
                tree_code_t* parent = create_node(T_MUL, 0, 0, create_empty(T_VAR, respect_to), NULL);

                parent_branch(tree->left, 2, parent);
                parent_branch(tree->right, 2, parent);

                break;
        }

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

                message(MESSAGE_DEBUG, "VARIABLE %f\n", last_token->value);
                
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

        tree_code_t *tree = create_node(((cont_loop & 1) ? T_MUL : T_DIV), 0, 0, left, right);
        
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

        tree_code_t *tree = create_node(((cont_loop & 1) ? T_ADD : T_SUB), 0, 0, left, right);
        
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