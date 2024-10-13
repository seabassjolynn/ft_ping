#ifndef DEBUG_H
#define DEBUG_H

#include <stdbool.h>

#include "ping_session.h"

extern bool g_debug;

void debug_printf(const char *format, ...);

void debug_print(const char *str);

void debug_print_flags();

#endif