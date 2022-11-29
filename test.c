#include <stdio.h>
#include <stdlib.h>

extern double do_the_float();

int main() {
        printf("%X\n", do_the_float());
        printf("%f\n", do_the_float());

        return 0;
}

double a(double b) {
        return 5.1 + b;
}
