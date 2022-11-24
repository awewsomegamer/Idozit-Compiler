#include <messages.h>
#include <design1.h>

void (*message_handler_function)(const char*, int, va_list) = NULL;

void _set_message_handler(void* handler) {
        message_handler_function = handler;
}

void message(const char* message, int level, ...) {
        va_list args;
        va_start(args, level);

        if (message_handler_function != NULL)
                (*message_handler_function)(message, level, args);

        switch (level) {
        case MESSAGE_FATAL:
                break;
        
        case MESSAGE_ERROR:
                break;

        case MESSAGE_WARNING:
                break;

        case MESSAGE_DEBUG:
                break;
        }
        
}
