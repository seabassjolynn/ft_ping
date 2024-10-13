#ifndef PING_SESSION_H
#define PING_SESSION_H

#include <sys/time.h>

#include <netinet/in.h> //INET_ADDRSTRLEN

#include "net.h" //ECHO_PACKET_BODY_LENGTH

#include <stdbool.h>
#define ERROR_REPLY_CODE_DESCRIPTION_BUFFER_SIZE 255

extern struct s_ping_session g_ping_session;

struct s_ping_data
{
    struct timeval time_start;
    struct timeval time_end;
    long received_bytes_count;
    long icmp_seq_number;
    char reply_host_str_addr[INET_ADDRSTRLEN];
    bool is_error_reply;
    char error_reply_code_description[ERROR_REPLY_CODE_DESCRIPTION_BUFFER_SIZE];
    struct s_error_ping_reply error_reply;
};

struct s_flags
{
    bool print_man_only;
    bool is_verbose;
    int ttl;
    struct timeval linger;
    int interval_between_pings_usec;
    bool is_debug;
    int count;
    int timeout;
};

struct s_ping_session
{
    char* target_arg;
    uint32_t id;
    char target_addr[INET_ADDRSTRLEN];
    uint64_t sent_echo_count;
    struct s_ping_data *ping_data_arr;
    int ping_data_arr_count;
    int ping_data_arr_next_index;
    struct s_flags flags;
    int timeout;
};

void init();

void print_icmp_echo_reply(struct s_ping_data *ping_data);

void print_icmp_error_reply(struct s_ping_data *ping_data);

void add_to_ping_session(struct s_ping_data *ping_data);

void print_ping_header();

void configure_socket();

#endif
