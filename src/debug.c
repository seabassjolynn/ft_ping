#include <stdbool.h>

#include <stdio.h>
#include <stdarg.h>

bool g_debug = false;

void debug_printf(bool debug_mode, const char *format, ...) 
{
    if (debug_mode) {
        va_list args;
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
    }
}