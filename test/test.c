#include <design1.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

int main() {
        char* variables[] = { "x", "y" };
        code_block_t code = compile(expression("x * y + 1", variables, sizeof(variables)/sizeof(variables[0])));

        double var_val[] = { 15, 2 };
	printf("Results in %f\n", run(code, var_val, sizeof(var_val)/sizeof(var_val[0])));

        return 0;
}