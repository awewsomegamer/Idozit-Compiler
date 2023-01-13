#include <idozit.h>

int main() {
        code_block_t code = compile(expression("INTEGRAL 2 x ( x )", 1, "x"));
        // TODO: Really annoying how run function doesn't like integers and only likes when the .0 is added
        printf("%f\n", run(&code, 2.0));

        return 0;
}