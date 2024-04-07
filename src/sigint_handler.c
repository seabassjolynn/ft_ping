//exit
#include <stdlib.h>
#include "exit_constants.h"
#include <stdio.h>
#include "ping_statistics.h"
#include "free_resources.h"

void handle_sigint(int sig)
{
    print_statistics();
    
    free_resources();    
    
    exit(EXIT_SUCCESS);
}