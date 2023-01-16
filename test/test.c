#include <idozit.h>

int main() {
        idozit_word.caching = 1;
        idozit_word.available_caches = 5;
        idozit_word.cache_cut_off = 60 * 60 * 5 * 1000; // 5 minutes

        code_block_t code = compile(expression("INTEGRAL 2 x ( x )", 1, "x"));
        // TODO: Really annoying how run function doesn't like integers and only likes when the .0 is added
        printf("%f\n", run(&code, 2.0));

        return 0;
}