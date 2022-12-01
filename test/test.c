#include <design1.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

int main() {
        char* variables[] = { "x" };
        code_block_t code = compile(expression("2 * x + 1", variables));

        double var_val[] = { 1 };
	printf("Results in %f\n", run(code, var_val));

        return 0;
}