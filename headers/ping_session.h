#ifndef PING_SESSION_H
#define PING_SESSION_H

#include <sys/time.h>

#include <netinet/in.h> //INET_ADDRSTRLEN

#include "net.h" //ECHO_PACKET_BODY_LENGTH

struct s_ping_data
{
    struct timeval time_start;
    struct timeval time_end;
    long received_bytes_count;
    long icmp_seq_number;
    char reply_host_str_addr[INET_ADDRSTRLEN];
};

struct s_ping_session
{
    char* host_name;
    char request_host_str_addr[INET_ADDRSTRLEN];
    struct s_ping_data *ping_data_arr;
    int ping_data_arr_count;
    int ping_data_arr_next_index;
    long ttl;
    struct timeval echo_reply_timeout;
    long interval_between_pings_usec;
};

extern struct s_ping_session g_ping_session;

void print_icmp_echo_reply(struct s_ping_data *ping_data);

void add_to_ping_session(struct s_ping_data *ping_data);

void print_ping_session_header();

#endif
