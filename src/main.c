#include <stdio.h>

//memeset
#include <string.h>

//inet_ntop
#include <arpa/inet.h>

//exit
#include <stdlib.h>

//errno
#include <errno.h>

//sleep
#include <unistd.h>

#include <stdbool.h>

//register signint handler
#include <signal.h>

#include "sigint_handler.h"

#include "exit_constants.h"

#include "ping.h"

#include "net.h"

#include "debug.h"

#include "ping_session.h"

int packet_sequence_number = 0;

#include "ping_session.h"
#include "resources.h"

#include "parse_args.h"

#include "ping_session_stat.h"

static void print_man() {
    printf("Usage: ping [OPTION...] HOST ...\n");
    printf("Send ICMP ECHO_REQUEST packets to network hosts.\n");
    printf("Options:\n");
    printf("-d, --debug              print debug info\n");
    printf("-v, --verbose            verbose output\n");
    printf("    --ttl=NUM            specify N as time-to-live\n");
    printf("-?, --help, --usage      give this help list\n");
    printf("-c, --count=NUMBER       stop after sending NUMBER packets\n");
    printf("-w, --timeout=N          stop after N seconds\n");
    printf("-W, --linger=N           number of seconds to wait for response\n");
}

int main(int ac, char **av) {
    
    init();
    
    parse_args(ac, av);
    
    debug_print_flags();
    
    if (g_ping_session.flags.print_man_only)
    {
        print_man();
        exit(EXIT_SUCCESS);
    }

    signal(SIGINT, handle_sigint);
    if (g_ping_session.flags.timeout != -1)
    {
        signal(SIGALRM, handle_sigalarm);
        alarm(g_ping_session.flags.timeout);
    }

    init_resouces();
    
    get_addr_info(g_ping_session.target_arg, &g_resources.target_addr_info);
    
    sockaddr_to_ipv4_addr_str(g_resources.target_addr_info->ai_addr, g_ping_session.target_addr);
    
    open_socket();
    
    configure_socket();

    struct s_icmp_echo_packet echo_request = create_echo_request();
    
    print_ping_header();
    
    g_ping_session.sent_echo_count = 0;
    
    int i = 0;
    
    while (g_ping_session.flags.count == -1 || i < g_ping_session.flags.count)
    {
        echo_request.header.sequence_number++;
        echo_request.header.checksum = 0; //For computing the checksum , the checksum field should be zero. 
        echo_request.header.checksum = caclulate_checksum(&echo_request, ECHO_PACKET_LENGTH);
        
        struct s_ping_data ping_data = ping(echo_request);
        
        g_ping_session.sent_echo_count++;

        if (ping_data.received_bytes_count != TIME_OUT && !ping_data.is_error_reply)
        {
            add_to_ping_session(&ping_data);
            print_icmp_echo_reply(&ping_data);
        } 
        else if (ping_data.is_error_reply)
        {
            print_icmp_error_reply(&ping_data);
        }
         
        usleep(g_ping_session.flags.interval_between_pings_usec);
        
        i++;
    }
    
    struct s_ping_session_stat ping_session_stat = calc_stat();
    
    print_statistics(&ping_session_stat);
    
    free_resources();
    return 0;
}
