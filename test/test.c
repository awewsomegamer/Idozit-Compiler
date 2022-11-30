#include <design1.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

int main() {
        code_block_t code = compile(expression("E * e * PI"));
        
	printf("Results in %f\n", run(code));

        return 0;
}