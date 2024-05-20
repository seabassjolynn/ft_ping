//exit
#include <stdlib.h>
#include "exit_constants.h"
#include <stdio.h>
#include "resources.h"
#include "ping_session_stat.h"

void handle_sigint(int sig)
{
    struct s_ping_session_stat ping_session_stat = calc_ping_session_stat(&g_ping_session);
    print_statistics(&ping_session_stat);
    
    free_resources();    
    
    exit(EXIT_SUCCESS);
}