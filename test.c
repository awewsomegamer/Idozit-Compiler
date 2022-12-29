#include <stdio.h>
#include <stdlib.h>

extern double do_the_float();
double a(double b);

int main() {
        printf("%X\n", do_the_float());
        printf("%f\n", do_the_float());

        a(15.0);

        return 0;
}

double a(double b) {
        return 5.1 + b;
}
