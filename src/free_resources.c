#include "free_resources.h"
#include <stdlib.h>

#include <unistd.h>

struct s_resources g_resources;

void init_resouces()
{
    g_resources.fd_socket = -1;
    g_resources.addr_info = NULL;
}

void free_resources()
{
    close(g_resources.fd_socket);
    freeaddrinfo(g_resources.addr_info);
}