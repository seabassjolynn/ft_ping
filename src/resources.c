#include "resources.h"
#include <stdlib.h>

#include <unistd.h>

#include "exit_constants.h"

struct s_resources g_resources;

void init_resouces()
{
    g_resources.fd_socket = -1;
    g_resources.target_addr_info = NULL;
    g_resources.ping_data_arr = NULL;
}

void free_resources()
{
    close(g_resources.fd_socket);
    freeaddrinfo(g_resources.target_addr_info);
    free(g_resources.ping_data_arr);
}