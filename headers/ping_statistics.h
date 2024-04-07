#ifndef PING_STATISTICS_H
#define PING_STATISTICS_H

struct s_statistics
{
    char* host_name;
};

extern struct s_statistics g_statistics;

void print_statistics();

#endif















