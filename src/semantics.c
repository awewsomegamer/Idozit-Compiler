#include <semantics.h>
#include <lexer.h>
#include <messages.h>

/* void validate(tree_code_t *head)
 * Recursively validate the given tree, head
 */
void validate(tree_code_t *head)
{
        switch (head->type) {
        case T_VAR:
        case T_INT:
        case T_NUMBER:
                if (!(head->left == NULL && head->right == NULL))
                        message(MESSAGE_ERROR, "Illegal children of %s\n", TOKEN_NAMES[head->type]);

                break;
        
        case T_ADD:
        case T_SUB:
        case T_MUL:
        case T_DIV:
        case T_EXPONENT:
                if (!(head->left != NULL && head->right != NULL))
                        message(MESSAGE_ERROR, "%s statement incomplete\n", TOKEN_NAMES[head->type]);

                break;
        }

        if (head->left != NULL) validate(head->left);
        if (head->right != NULL) validate(head->right);
}
