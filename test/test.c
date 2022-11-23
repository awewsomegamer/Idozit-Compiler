#include <design1.h>

int main() {
        context_t exp = expression("(2 + 3 + (2 * (6 * 12))) * 1.5");
        context_t exp2 = expression("5 + 5");

        return 0;
}