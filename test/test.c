#include <idozit.h>

int main() {
        code_block_t code = compile(expression("INTEGRAL 1 x ( x )", 1, "x"));
        // TODO: Really annoying how run function doesn't like integers and only likes when the .0 is added
        printf("%f\n", run(code, 1.0));
        // printf("Area under x from -1 to 2 is: %f\n", (run(code, 2.0) - run(code, -1.0)));

        return 0;
}