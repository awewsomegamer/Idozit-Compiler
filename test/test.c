#include <design1.h>

int main() {
        code_block_t code = compile(expression("INTEGRAL 1 x ( x^2 - 4 )", 1, "x"));
	printf("Area under x^2 - 4 from -2 to 2 is: %f\n", (run(code, 2.0) - run(code, -2.0)));

        return 0;
}