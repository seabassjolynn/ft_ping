#include <stdio.h>

//getaddrinfo
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

//memeset
#include <string.h>

//inet_ntop
#include <arpa/inet.h>

//exit
#include <stdlib.h>

//errno
#include <errno.h>

//socket
#include <sys/socket.h>

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

#include "parse_flags.h"

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
    set_defaults(&g_ping_session.flags);
    
    parse_flags(ac, av, &g_ping_session.flags);

    debug_print_flags(g_ping_session.flags.is_debug, &g_ping_session.flags);
    
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

    bzero(g_ping_session.request_host_str_addr, INET_ADDRSTRLEN);
    g_ping_session.ping_data_arr_count = 0;
    g_ping_session.ping_data_arr_next_index = 0;
    g_ping_session.flags.interval_between_pings_usec = 1000000;
    g_ping_session.id = getpid();
    init_resouces();
    //--------------------------resolve host passed as an argument
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // Allow IPv4
    hints.ai_socktype = SOCK_RAW; // Raw socket for ICMP
    hints.ai_protocol = IPPROTO_ICMP; // ICMP protocol
    
    int result = getaddrinfo(g_ping_session.host_from_args, NULL, &hints, &g_resources.addr_info);

    if (result != 0) {
        fprintf(stderr, "%s: unknown host\n", av[0]);
        free_resources();
        exit(EXIT_ERROR);
    }
    
    //here i cast struct sockaddr* to struct sockaddr_in* because ai_addr may contain different address structures: struct sockaddr_in* - for ipv4, struct sockaddr_in6* for ipv6
    struct sockaddr_in remote_host_net_addr = *((struct sockaddr_in*) (g_resources.addr_info)->ai_addr);
    
    get_str_ip_addr_from_in_addr(&(remote_host_net_addr.sin_addr), g_ping_session.request_host_str_addr);
    
    // if(inet_ntop(AF_INET,&(remote_host_net_addr.sin_addr), remote_host_str_addr, INET_ADDRSTRLEN) == NULL) {
    //     fprintf(stderr, "inet_ntop: %s\n", strerror(errno));
    //     free_resources();
    //     exit(EXIT_ERROR);
    // }
    
    //---------------------open socket
    g_resources.fd_socket = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (g_resources.fd_socket == -1) {
        perror("socket");
        free_resources();
        exit(EXIT_ERROR);
    }

    if (setsockopt(g_resources.fd_socket, SOL_SOCKET, SO_RCVTIMEO, (const void*)&g_ping_session.flags.linger, sizeof(struct timeval)) != 0)
    {
        perror("setsockopt");
        free_resources();
        exit(EXIT_FAILURE);
    }

    if (setsockopt(g_resources.fd_socket, IPPROTO_IP, IP_TTL, &g_ping_session.flags.ttl, sizeof(g_ping_session.flags.ttl)) != 0) {
        perror("setsockopt");
        free_resources();
        exit(EXIT_FAILURE);
    }
    //----------------------create packet;
    struct s_icmp_echo_packet icmp_request_packet = create_icmp_echo_request_packet();
    
    //-------------------- print ping header
    print_ping_session_header();
    
    g_ping_session.sent_echo_count = 0;
    int i = 0;
    
    while (g_ping_session.flags.count == -1 || i < g_ping_session.flags.count)
    {
        icmp_request_packet.header.sequence_number++;
        icmp_request_packet.header.checksum = 0; //For computing the checksum , the checksum field should be zero. 
        icmp_request_packet.header.checksum = caclulate_checksum(&icmp_request_packet, ECHO_PACKET_LENGTH);
        
        struct s_ping_data ping_data = ping(icmp_request_packet);
        
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
        //printf("Round trip time microsec %d\n", round_trip_time_microsec);
        //calculate and print statistics.
        //why first ping is from 0.0.0.0?
        //may be if received sock addr len is 0 or addr is 0,0.0.0 I just use request addr?
        //why my ping returns another quantity of bytes?
        i++;
    }
    
    struct s_ping_session_stat ping_session_stat = calc_ping_session_stat(&g_ping_session);
    
    print_statistics(&ping_session_stat);
    
    free_resources();
    return 0;
}
// in order to see how my ping works in case of lost packets, I have to 
// modify timeout, lower it

//Handle situation when there is no internet conncection. it looks like getaddress blocks!!!
//calc checksum when ping is received back
//don't forget sudo, so ping works without sudo


//next step is to figure out why hex line is different from real ping hex line