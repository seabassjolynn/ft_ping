//exit
#include <stdlib.h>
#include "exit_constants.h"
#include <stdio.h>
#include "resources.h"
#include "ping_session_stat.h"
#include <signal.h>

void handle_sigint(int sig)
{
    (void)sig; 
    
    struct s_ping_session_stat ping_session_stat = calc_stat(g_ping_session);
    
    print_statistics(&ping_session_stat);
    
    free_resources();    
    
    exit(EXIT_SUCCESS);
}

void handle_sigalarm(int sig)
{
    (void)sig;
    raise(SIGINT);
}