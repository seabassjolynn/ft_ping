#include <stdbool.h>

#include <stdio.h>
#include <stdarg.h>

#include "ping_session.h"

void debug_printf(const char *format, ...)
{
    if (g_ping_session.flags.is_debug) {
        va_list args;
        va_start(args, format);
        printf("DEBUG: ");
        vprintf(format, args);
        va_end(args);
    }
}

void debug_print(const char *str)
{
    if (g_ping_session.flags.is_debug) {
        printf("DEBUG: ");
        printf(str);
    }
}

void debug_print_flags()
{
    debug_print("---program flags---\n");
    debug_printf("-v, --verbose = %s\n", g_ping_session.flags.is_verbose ? "true" : "false");
    debug_printf("-? = %s\n", g_ping_session.flags.print_man_only ? "true" : "false");
    debug_printf("--ttl = %d\n", g_ping_session.flags.ttl);
    debug_printf("-d --debug = %s\n", g_ping_session.flags.is_debug ? "true" : "false");
    debug_printf("--count = %d\n", g_ping_session.flags.count);
    debug_printf("--timeout = %d\n", g_ping_session.flags.timeout);
    debug_printf("--linger = %d\n", g_ping_session.flags.linger.tv_sec);
}