#include <idozit.h>
#include <unistd.h>

int main() {
        idozit_word.caching = 1;
        idozit_word.available_caches = 5;
        idozit_word.cache_cut_off = 1000 * 60 * 5; // 5 minutes

        code_block_t code0 = compile(expression("INTEGRAL 1 x ( x^2 + 2*x + 1 + y + y*x )", 2, "x", "y"));
        // code_block_t code1 = compile(expression("2", 0));
        // code_block_t code2 = compile(expression("3", 0));
        // code_block_t code3 = compile(expression("4", 0));
        // code_block_t code4 = compile(expression("5", 0));
        // code_block_t code5 = compile(expression("6", 0));
        // code_block_t code6 = compile(expression("7", 0));
        // code_block_t code7 = compile(expression("8", 0));

        // // TODO: Really annoying how run function doesn't like integers and only likes when the .0 is added
        printf("%f\n", run(&code0, 2.0, 1.0));
        // run(&code1, 2.0);
        // run(&code2, 2.0);
        // run(&code3, 2.0);
        // run(&code4, 2.0);
        // run(&code5, 2.0);
        // run(&code6, 2.0);
        // run(&code7, 2.0);

        // while (1) {
        //         run(&code5, 2.0);
        //         sleep(1);
        // }

        return 0;
}