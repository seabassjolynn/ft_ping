#include "ping_statistics.h"

//NULL definition
#include "stdlib.h"

#include "exit_constants.h"

//prinf
#include <stdio.h>

struct s_statistics g_statistics;

void print_statistics()
{
    printf("--- %s ping statistics ---\n", g_statistics.host_name);
}






















