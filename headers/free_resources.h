#ifndef FREE_RESOURCES_H
#define FREE_RESOURCES_H

//addrinfo
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

struct s_resources
{
    int fd_socket;
    struct addrinfo *addr_info;
};

extern struct s_resources g_resources;

void free_resources();

void init_resouces();

#endif