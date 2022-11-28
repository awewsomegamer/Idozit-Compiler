#include <design1.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

int main() {
        code_block_t code = compile(expression("5 + 5 + 6"));
        
        void* buf;
	buf = mmap(0, code.size, PROT_READ | PROT_WRITE | PROT_EXEC,MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	memcpy(buf, code.func, code.size);

	int result = ((int (*) (void))buf)();
	printf("Results in %d\n", result);

        return 0;
}