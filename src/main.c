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



int main(int ac, char **av) {
    
    if (ac != 3) {
        printf("Expect 2 arguments: host name and debug flag.");
    }

    if (strcmp(av[2], "true") == 0)
    {
        g_debug = true;
    }

    signal(SIGINT, handle_sigint);
    g_ping_session.host_name = av[1];
    g_ping_session.ttl = 63;
    g_ping_session.echo_reply_timeout.tv_sec = 1;
    g_ping_session.echo_reply_timeout.tv_usec = 0;
    bzero(g_ping_session.request_host_str_addr, INET_ADDRSTRLEN);
    g_ping_session.ping_data_arr_count = 0;
    g_ping_session.ping_data_arr_next_index = 0;
    g_ping_session.interval_between_pings_usec = 1000000;

    init_resouces();
    //--------------------------resolve host passed as an argument
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // Allow IPv4
    hints.ai_socktype = SOCK_RAW; // Raw socket for ICMP
    hints.ai_protocol = IPPROTO_ICMP; // ICMP protocol
    
    int result = getaddrinfo(av[1], NULL, &hints, &g_resources.addr_info);
    if (result != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(result)); //real ping prints: ping: host unknown in case of blabla host
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
        fprintf(stderr, "socket: %s\n", strerror(errno));
        free_resources();
        exit(EXIT_ERROR);
    }
    
    setsockopt(g_resources.fd_socket, SOL_SOCKET, SO_RCVTIMEO, (const void*)&g_ping_session.echo_reply_timeout, sizeof(struct timeval));
    
    //----------------------create packet;
    struct s_icmp_echo_packet icmp_request_packet = create_icmp_echo_request_packet();
    
    //-------------------- print ping header
    print_ping_session_header();
    
    while (true)
    {
        icmp_request_packet.header.sequence_number++;
        icmp_request_packet.header.checksum = 0; //For computing the checksum , the checksum field should be zero. 
        icmp_request_packet.header.checksum = caclulate_checksum(&icmp_request_packet, ECHO_PACKET_LENGTH);
    
        struct s_ping_data ping_data = ping(icmp_request_packet);
        
        add_to_ping_session(&ping_data);
        
        if (ping_data.received_bytes_count != -1)
        {
            print_icmp_echo_reply(&ping_data);
        }
        
        usleep(g_ping_session.interval_between_pings_usec);
        //printf("Round trip time microsec %d\n", round_trip_time_microsec);
        //calculate and print statistics.
        //why first ping is from 0.0.0.0?
        //may be if received sock addr len is 0 or addr is 0,0.0.0 I just use request addr?
        //why my ping returns another quantity of bytes?
    }
    return 0;
}
