#include <idozit.h>
#include <string.h>

int main(int argc, char** argv) {
        // argv[1] = expression 1
        // argv[2] = interval from
        // argv[3] = interval to
        // variable is x
        
        char* expr_string = malloc(strlen(argv[1]) + strlen("INTEGRAL 1 x (  )") + 1);
        sprintf(expr_string, "INTEGRAL 1 x ( %s )", argv[1]);

        code_block_t code = compile(expression(expr_string, 1, "x"));
        printf("The area under the curve %s is %f.\n", argv[1], (run(code, atof(argv[3]) - run(code, atof(argv[2])))));
}