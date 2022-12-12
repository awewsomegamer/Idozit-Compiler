#ifndef MESSAGES_H
#define MESSAGES_H

#include <stdarg.h>

enum {
        MESSAGE_FATAL,
        MESSAGE_ERROR,
        MESSAGE_WARNING,
        MESSAGE_DEBUG,
};

static const char* MESSAGE_NAMES[] = {
        [MESSAGE_FATAL]     = "FATAL ERROR",
        [MESSAGE_ERROR]     = "ERROR",
        [MESSAGE_WARNING]   = "WARNING",
        [MESSAGE_DEBUG]     = "DEBUG"
};

extern void (*message_handler_function)(int, const char*, va_list);

/* void _set_message_handler(void (*handler)(int, const char*, va_list)) :
 * The internal function to set the message_handler_function variable
 */
void _set_message_handler(void (*handler)(int, const char*, va_list));

/* void message(int level, const char *message, ...) :
 * Prints the given message with the given attributes (...) on the given
 * level
 */
void message(int level, const char *message, ...);

#endif