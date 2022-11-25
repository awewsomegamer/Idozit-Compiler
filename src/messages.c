#include <messages.h>
#include <design1.h>

void (*message_handler_function)(const char*, int, va_list) = NULL;

void _set_message_handler(void* handler) {
        message_handler_function = handler;
}

void message(int level, const char* message, ...) {
        va_list args;
        va_start(args, level);

        if (level == MESSAGE_DEBUG && !DEBUG)
                return;

        if (message_handler_function != NULL) {
                (*message_handler_function)(message, level, args);
                return;
        }

        printf("[%s]: ", MESSAGE_NAMES[level]);
        vprintf(message, args);

        if (level == MESSAGE_FATAL)
                exit(1);
}
