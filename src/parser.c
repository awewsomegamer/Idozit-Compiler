#include <parser.h>
#include <lexer.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <messages.h>

token_t *current_token = NULL;
token_t *last_token = NULL;

tree_code_t * (*parse_function)() = NULL;

// Functions to manipulate nodes
tree_code_t *create_node(int type, double value, uint64_t parser_mark, tree_code_t *left, tree_code_t *right)
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

        const size_t length = (strlen(expression_string) <= 20 ? strlen(expression_string) : 20);
        char *section = malloc(length + 21);
        memset(section, ' ', length + 21);
        memcpy(section, expression_string + expression_ptr - (length / 2 + 1), length);

        for (int i = 0; i < length; i++)
                if (*(section + i) == 0)
                        *(section + i) = ' ';

        *(section + 20) = '\n';
        *(section + 21 + length / 2) = '^';

        message(MESSAGE_FATAL, "Expected token type %d (%s) instead of %d (%s) at character %d\n%s\n", 
        type, TOKEN_NAMES[type], current_token->type, TOKEN_NAMES[current_token->type], expression_ptr + 1,
        section);
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

/* tree_code_t *find_node(tree_code_t *tree, int type, double value, uint64_t parser_mark) :
 * This function will look throuhg the given tree for any node with the given specifications.
 * Specifications:
 *      -> Type of node
 *      -> Value of the node
 *      -> Parser mark of the node
 */
tree_code_t *find_node(tree_code_t *tree, int type, double value, uint64_t parser_mark)
{
        if (tree != NULL && tree->type == type && tree->value == value && tree->parser_mark == parser_mark)
                return tree;

        tree_code_t *branch = NULL;

        if (tree->left != NULL) branch = find_node(tree->left, type, value, parser_mark);
        if (branch != NULL) return branch;
        
        if (tree->right != NULL) branch = find_node(tree->right, type, value, parser_mark);
        if (branch != NULL) return branch;

        return NULL;
}

/* void tree_set_value(tree_code_t *tree, uint64_t mark, double value) :
 * Recursively sets the value of the given tree to the given value if the
 * tree node's parser mark does not meet the given parser mark
 */
void tree_set_value(tree_code_t *tree, uint64_t mark, double value)
{
        if (tree->parser_mark != mark) {
                tree->value = value;

                if (tree->left != NULL) tree_set_value(tree->left, mark, value);
                if (tree->right != NULL) tree_set_value(tree->right, mark, value);
        }
}

/* void reset_parser_marks(tree_code_t *tree) :
 * Recursively sets the parser mark of the given
 * tree to 0.
 */
void reset_parser_marks(tree_code_t *tree)
{
        if (tree->left != NULL) reset_parser_marks(tree->left);
        if (tree->right != NULL) reset_parser_marks(tree->right);
        tree->parser_mark = 0;
}

/* uint8_t tree_homogenous(tree_code_t *tree, uint64_t mark) :
 * This function checks to see if the parser mark of the nodes
 * descending down the entirety of the given tree are the same,
 * returning a 1 if this is the case and a 0 if not.
 */
uint8_t tree_homogenous(tree_code_t *tree, uint64_t mark)
{
        uint8_t left = 1, right = 1;

        if (tree->left != NULL) left = tree_homogenous(tree->left, mark);
        if (tree->right != NULL) right = tree_homogenous(tree->right, mark);

        if (tree != NULL && tree->parser_mark != mark)
                return 0;

        return left * right;
}

/* void parent_branch(tree_code_t *tree, uint64_t mark, uint64_t homogenous_mark, tree_code_t *parent) :
 * This function will recursively parent the given tree to the given parent if it does not meet the
 * parser mark, but does meet the homogenous mark.
 */
void parent_branch(tree_code_t *tree, uint64_t mark, uint64_t homogenous_mark, tree_code_t *parent)
{
        int homogenous = 0;
        

        if (tree != NULL && tree->parser_mark != mark) {
                homogenous = tree_homogenous(tree, homogenous_mark);
                
                tree_code_t *tmp = create_node(tree->type, tree->value, tree->parser_mark, tree->left, tree->right);
                
                tree->type = parent->type;
                tree->value = parent->value;
                tree->parser_mark = parent->parser_mark;

                if (parent->left == NULL) {
                        tree->left = tmp;
                        tree->right = create_node(parent->right->type, parent->right->value, parent->right->parser_mark,
                                                  parent->right->left, parent->right->right);
                } else if (parent->right == NULL) {
                        tree->right = tmp;
                        tree->left = create_node(parent->left->type, parent->left->value, parent->left->parser_mark,
                                                  parent->left->left, parent->left->right);
                }
        }

        if (!homogenous && tree->left != NULL) parent_branch(tree->left, mark, homogenous_mark, parent);
        if (!homogenous && tree->right != NULL) parent_branch(tree->right, mark, homogenous_mark, parent);
}

/* void apply_function(int function, int degree, int respect_to, tree_code_t *tree) :
 * Applies the given mathematical function to the given tree with respect to the given
 * given variable.
 */
void apply_function(int function, int degree, int respect_to, tree_code_t *tree)
{
        tree_code_t *var, *opposite, *parent, *new_node;
        
        switch (function) {
        case T_FUNC_DERIVATIVE: {
                do {
                        var = find_node(tree, T_VAR, respect_to, 0);

                        if (var == NULL) break;

                        parent = var->parent;

                        if (parent != NULL) {
                                if (var == parent->left) opposite = parent->right;
                                if (var == parent->right) opposite = parent->left; 
                        }

                        if (parent != NULL && parent->type == T_EXPONENT) {
                                int value = evaluate_tree(parent->right) - 1;

                                if (value > 1) {
                                        // var ^ (value) * (value + 1)
                                        free_tree(parent->left);
                                        free_tree(parent->right);

                                        parent->type = T_MUL;

                                        parent->left = create_node(T_EXPONENT, 0, 0, create_empty(T_VAR, respect_to), create_empty(T_INT, value));
                                        parent->left->left->parser_mark = 1;

                                        parent->right = create_empty(T_INT, (value + 1));

                                        parent->parser_mark = 2;
                                } else if (value == 1) {
                                        // (value + 1) * var
                                        free_tree(parent->left);
                                        free_tree(parent->right);

                                        parent->type = T_MUL;
                                        parent->left = create_empty(T_VAR, respect_to);
                                        parent->left->parser_mark = 1;

                                        parent->right = create_empty(T_INT, (value + 1));
                                        parent->parser_mark = 2;
                                } else {
                                        // (value + 1)
                                        free_tree(parent->left);
                                        free_tree(parent->right);
                                        parent->right = NULL;
                                        parent->left = NULL;

                                        parent->type = T_INT;
                                        parent->value = value + 1;
                                        parent->parser_mark = 2;
                                }
                        } else if (parent != NULL && parent->type == T_MUL) {
                                parent->type = opposite->type;
                                parent->value = opposite->value;
                                parent->parser_mark = 2;

                                free_tree(parent->left);
                                free_tree(parent->right);
                                parent->left = NULL;
                                parent->right = NULL;
                        } else {
                                var->type = T_INT;
                                var->value = 1;
                                var->parser_mark = 2;
                        }

                        while (parent != NULL && (parent->type == T_MUL || parent->type == T_DIV)) {
                                parent->parser_mark = 2;
                                parent = parent->parent;
                        }
                } while (var != NULL);

                tree_set_value(tree, 2, 0);

                break;
        }

        case T_FUNC_INTEGRAL: {
                do {   
                        // Find the variables
                        var = find_node(tree, T_VAR, respect_to, 0);

                        if (var == NULL) break;

                        parent = var->parent;

                        if (parent != NULL) {
                                if (var == parent->left) opposite = parent->right;
                                if (var == parent->right) opposite = parent->left;
                        }

                        if (parent != NULL && parent->type == T_EXPONENT) {
                                // x^2 -> x^3/3
                                int value = evaluate_tree(parent->right) + 1;

                                parent->type = T_DIV;
                                parent->left = create_node(T_EXPONENT, 0, 3, create_empty(T_VAR, var->value), create_empty(T_INT, value));
                                parent->right = create_empty(T_INT, value);
                                parent->left->left->parser_mark = 1;

                                parent->left->parser_mark = 3;
                                parent->left->left->parser_mark = 3;
                                parent->left->right->parser_mark = 3;
                                parent->right->parser_mark = 3;
                                parent->parser_mark = 3; 
                        } else if (parent != NULL && parent->type == T_MUL) {
                                // 2 * x -> x^2, 3 * x^2 -> x^3, x*x -> x^2/2

                                if (opposite->type == T_INT || opposite->type == T_NUMBER) {
                                        opposite->value = evaluate_tree(opposite) / 2;
                                        opposite->type = T_NUMBER;
                                }
                                
                                if (opposite == parent->left) {
                                        parent->left = var;
                                        parent->right = opposite;
                                }

                                if (opposite->value == 1) {
                                        parent->value = var->value;
                                        var = parent;
                                }

                                goto REG_GEN;
                        } else {
                                REG_GEN:
                                var->type = T_DIV;
                                var->left = create_node(T_EXPONENT, 0, 3, create_empty(T_VAR, var->value), create_empty(T_INT, 2));
                                var->left->left->parser_mark = 3;
                                var->left->right->parser_mark = 3;

                                var->right = create_empty(T_INT, 2);
                                var->right->parser_mark = 3;
                                var->parser_mark = 3;
                                // var->type = T_EXPONENT;
                                // var->left = create_empty(T_VAR, var->value);
                                // var->right = create_empty(T_INT, 2);
                                
                                // var->parser_mark = 3;       
                                // var->left->parser_mark = 3;
                                // var->right->parser_mark = 3;
                        }

                        while (parent != NULL) {
                                parent->parser_mark = 3;
                                parent = parent->parent;
                        }
                } while (var != NULL);

                tree_code_t* parent_t = create_node(T_MUL, 0, 0, create_empty(T_VAR, respect_to), NULL);
                parent_branch(tree, 3, 0, parent_t);

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
                
                reset_parser_marks(ret);

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

tree_code_t *build_tree()
{
        if (parse_function != NULL)
                return (*parse_function)();

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

void _set_parser_function(tree_code_t * (*func)()) 
{
        parse_function = func;
}