#include <stdbool.h>

#include <stdio.h>
#include <stdarg.h>

void debug_printf(bool debug_mode, const char *format, ...)
{
    if (debug_mode) {
        va_list args;
        va_start(args, format);
        printf("DEBUG: ");
        vprintf(format, args);
        va_end(args);
    }
}

void debug_print(bool debug_mode, const char *str)
{
    if (debug_mode) {
        printf("DEBUG: ");
        printf(str);
    }
}