#include <design1.h>

void message_handler(int level, const char* format, va_list args) {
        printf("Hello Message Handling in a custom way:\n");
        vprintf(format, args);
        printf("\n");
}

int main() {
        // expression("4*4 * 3.14159");

        set_message_handler(message_handler);

        expression("5 + 5");
        expression("(");

        return 0;
}