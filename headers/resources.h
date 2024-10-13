#ifndef RESOURCES_H
#define RESOURCES_H

//addrinfo
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "ping_session.h"

struct s_resources
{
    int fd_socket;
    struct addrinfo *target_addr_info;
    void *ping_data_arr;
};

extern struct s_resources g_resources;

void free_resources();

void init_resouces();

#endif