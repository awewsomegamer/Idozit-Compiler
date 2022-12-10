#include <design1.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

int main() {
        code_block_t code = compile(expression("INTEGRAL 1 x ( x + 1 )", 2, "x", "y"));
	printf("Results in %f\n", run(code, 16.0, 2.0));

        return 0;
}