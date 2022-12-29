#include <idozit.h>

int main() {
        code_block_t code = compile(expression("INTEGRAL 1 x ( x )", 1, "x"));
	while (1)
                printf("Area under x from -1 to 2 is: %f\n", (run(code, 2.0) - run(code, -1.0)));

        return 0;
}