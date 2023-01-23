#include <messages.h>
#include <idozit.h>

void (*message_handler_function)(int, const char*, va_list) = NULL;

void _set_message_handler(void (*handler)(int, const char*, va_list))
{
        message_handler_function = handler;
}

void message(int level, const char* message, ...)
{
        va_list args;
        va_start(args, level);

        #ifndef DEBUG
        if (level == MESSAGE_DEBUG)
                return;
        #endif

        if (message_handler_function != NULL) {
                (*message_handler_function)(level, message, args);
                return;
        }

        printf("[%s]: ", MESSAGE_NAMES[level]);
        vprintf(message, args);

        if (level == MESSAGE_FATAL)
                exit(1);
}
