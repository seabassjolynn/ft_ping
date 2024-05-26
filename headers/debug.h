#ifndef DEBUG_H
#define DEBUG_H

#include <stdbool.h>

extern bool g_debug;

void debug_printf(bool debug_mode, const char *format, ...);

void debug_print(bool debug_mode, const char *str);

#endif