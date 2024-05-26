#include "ping_session_stat.h"
#include "ping_session.h"

#include <math.h> //pow, sqrt
#include <stdio.h> //printf
#include "debug.h"
float calc_round_trip_time_ms(struct timeval *start, struct  timeval *end)
{
    return (float)((end->tv_sec - start->tv_sec) * 1000000 + (end->tv_usec - start->tv_usec)) / 1000.0;
}

static float cacl_round_trip_stddev(struct s_ping_session *ping_session, float round_trip_average)
{   
    float sum_of_squared_diffs_from_average = 0;
    int round_trips_count = 0;
    
    int i = 0;
    while (i < ping_session->ping_data_arr_next_index)
    {
        if (ping_session->ping_data_arr[i].received_bytes_count != -1)
        {
            float current_round_trip_time = calc_round_trip_time_ms(&ping_session->ping_data_arr[i].time_start, &ping_session->ping_data_arr[i].time_end);
            sum_of_squared_diffs_from_average += pow(current_round_trip_time - round_trip_average, 2);
            round_trips_count += 1;    
        }
        i++;
    }
    return sqrt(sum_of_squared_diffs_from_average / round_trips_count);
}

static int calc_lost_packets_percentage(int tramsmitted_packets_count, int received_packets_count)
{
    
    debug_printf(g_ping_session.flags.is_debug, "calculate lost packets. Transmitted: %d, received: %d\n", tramsmitted_packets_count, received_packets_count);
    
    return ((float)(tramsmitted_packets_count - received_packets_count) / (float)tramsmitted_packets_count) * 100;
}

struct s_ping_session_stat calc_ping_session_stat(struct s_ping_session *ping_session)
{
    struct s_ping_session_stat ping_session_stat;
    
    ping_session_stat.round_trip_min_ms = ping_session->flags.echo_reply_timeout.tv_sec * 1000 + ping_session->flags.echo_reply_timeout.tv_usec / 1000;
    ping_session_stat.round_trip_max_ms = 0;
    ping_session_stat.round_trip_average = 0;
    ping_session_stat.round_trips_stddev = 0;
    ping_session_stat.lost_packets_percentage = 100;
    ping_session_stat.transmitted_packets_count = ping_session->ping_data_arr_next_index;
    ping_session_stat.received_packets_count = 0;

    float round_trips_ms_sum = 0;
    int echo_replies_count = 0;
    
    int i = 0;
    
    while (i < ping_session->ping_data_arr_next_index)
    {
        bool is_received = ping_session->ping_data_arr[i].received_bytes_count != -1;
        bool is_echo_reply = !ping_session->ping_data_arr[i].is_error_reply;
        
        if (is_received && is_echo_reply)
        {
            echo_replies_count++;
       
            float current_round_trip_time = calc_round_trip_time_ms(&ping_session->ping_data_arr[i].time_start, &ping_session->ping_data_arr[i].time_end);
       
            round_trips_ms_sum += current_round_trip_time;

            if (current_round_trip_time < ping_session_stat.round_trip_min_ms)
            {
                ping_session_stat.round_trip_min_ms = current_round_trip_time;
            }
            
            if (current_round_trip_time > ping_session_stat.round_trip_max_ms)
            {
                ping_session_stat.round_trip_max_ms = current_round_trip_time;
            } 
       }
       i++;
    }

    ping_session_stat.received_packets_count = echo_replies_count;

    if (ping_session_stat.transmitted_packets_count > 0)
    {
        ping_session_stat.lost_packets_percentage = calc_lost_packets_percentage(ping_session->ping_data_arr_next_index, echo_replies_count);
    }
    
    if (echo_replies_count > 0)
    {
        ping_session_stat.round_trip_average = round_trips_ms_sum / echo_replies_count;
        ping_session_stat.round_trips_stddev = cacl_round_trip_stddev(ping_session, ping_session_stat.round_trip_average);
    }
    return ping_session_stat;
}

void print_statistics(struct s_ping_session_stat *ping_session_stat)
{
    printf("--- %s ping statistics ---\n", g_ping_session.host_from_args);
    //3 packets transmitted, 0 packets received, 100% packet loss
    printf("%d packets transmitted, %d packets received, %d%% packet loss\n", 
        ping_session_stat->transmitted_packets_count, 
        ping_session_stat->received_packets_count, 
        ping_session_stat->lost_packets_percentage);
    
    if (ping_session_stat->received_packets_count != 0)
    {
        printf("round-trip min/avg/max/stddev = %.3f/%.3f/%.3f/%.3f ms\n", 
            ping_session_stat->round_trip_min_ms, 
            ping_session_stat->round_trip_average, 
            ping_session_stat->round_trip_max_ms, 
            ping_session_stat->round_trips_stddev);
    }
}
