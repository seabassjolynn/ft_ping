#include "ping_session.h"

//NULL definition
#include "stdlib.h"

#include "exit_constants.h"

//prinf
#include <stdio.h>
#include "resources.h"
#include <string.h> //memcpy
#include "debug.h"
#include "ping_session_stat.h" //calc_round_trip_time_ms

struct s_ping_session g_ping_session;

void print_ping_session_header()
{
    printf("PING %s (%s): %d data bytes\n", g_ping_session.host_name, g_ping_session.request_host_str_addr, ECHO_PACKET_BODY_LENGTH);
}

void print_icmp_echo_reply(struct s_ping_data *ping_data)
{
    float time = calc_round_trip_time_ms(&ping_data->time_start, &ping_data->time_end);

    printf("%d bytes from %s: icmp_seq=%d ttl=%ld time=%.3f\n", ping_data->received_bytes_count, ping_data->reply_host_str_addr, ping_data->icmp_seq_number, g_ping_session.ttl, time);
}

void add_to_ping_session(struct s_ping_data *ping_data)
{
    debug_printf(g_debug, "add_to_ping_session. Data count: %d, index: %d\n", g_ping_session.ping_data_arr_count, g_ping_session.ping_data_arr_next_index);
    
    if (g_ping_session.ping_data_arr_count == g_ping_session.ping_data_arr_next_index)
    {
        g_ping_session.ping_data_arr_count += 10;
        
        debug_printf(g_debug, "New data count: %d\n", g_ping_session.ping_data_arr_count);

        g_ping_session.ping_data_arr = realloc(g_ping_session.ping_data_arr, sizeof(struct s_ping_data) * g_ping_session.ping_data_arr_count);
        if (g_ping_session.ping_data_arr == NULL)
        {
            free_resources();
            exit(EXIT_ERROR);   
        }
        g_resources.ping_data_arr = g_ping_session.ping_data_arr;
    }
    
    memcpy(g_ping_session.ping_data_arr + g_ping_session.ping_data_arr_next_index, ping_data, sizeof(struct s_ping_data));
    g_ping_session.ping_data_arr_next_index++;
}



// when socket is created - set timeout for socket and ttl. These 2 things should be set outside ping function, because this setups are applicable to all pings in ping session
// not a concrete ping
// rename s_pinging_statistics to s_ping_session_statistics


















