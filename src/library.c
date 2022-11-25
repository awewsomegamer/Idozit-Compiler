#include <design1.h>
#include <lexer.h>
#include <parser.h>
#include <semantics.h>
#include <messages.h>

/* context_t expression(const char*, ...) :
 * Will take the given string and return it in a
 * context_t. This context_t will contain a list
 * of the variables and will also contain a pointer
 * to the head node of the generated tree code.
 * 
 * const char* : The expression to be converted into tree code
 * ...         : The variables (order will dictate calling order)
 *                             ( expression("x + y", "x", "y") will
 *                               will result int the generated function
 *                               being called as func(x, y);            )
 */
context_t expression(const char *form, ...) {
        expression_string = form;
        variables_list = NULL;
        variable_count = 0;
        expression_ptr = 0;
        
        tree_code_t* tree = build_tree();
        validate(tree);

        printf("%s = %f\n", form, evaluate_tree(tree));
}

/* void set_error_handler(void*) :
 * This function will set the error handler
 * to the given void* (function).
 */
void set_error_handler(void* func) {
       _set_message_handler(func); 
}

/* void default_error_handler() :
 * This function will revert the error handler
 * to the default error handler. 
 */
void default_error_handler() {
        _set_message_handler(NULL);
}