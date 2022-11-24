#ifndef MESSAGES_H
#define MESSAGES_H

#include <stdarg.h>

enum {
        MESSAGE_FATAL,
        MESSAGE_ERROR,
        MESSAGE_WARNING,
        MESSAGE_DEBUG,
};

static const char* ERROR_NAMES[] = {
        [MESSAGE_FATAL]     = "FATAL ERROR",
        [MESSAGE_ERROR]     = "ERROR",
        [MESSAGE_WARNING]   = "WARNING",
        [MESSAGE_DEBUG]     = "DEBUG"
};

extern void (*message_handler_function)(const char*, int, va_list);

void _set_message_handler(void* handler);
void message(const char* message, int level, ...);

#endif