#include <design1.h>
#include <lexer.h>
#include <parser.h>

context_t expression(const char *form, ...) {
        expression_string = form;
        variables_list = NULL;
        variable_count = 0;
        
        tree_code_t* tree = build_tree();

        printf("%s = %f\n", form, evaluate_tree(tree, 'H'));
}
