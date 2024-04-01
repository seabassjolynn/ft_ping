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

//get_pid()
//sleep
#include <unistd.h>

//true - false
#include <stdbool.h>

#include "icmp_echo_packet.h"

#define EXIT_ERROR_CODE 1

#define PING_PACKET_SIZE 56

#define ECHO_REQUEST_TYPE 8

#define PACKET_LENGTH 56

#define ECHO_REQUEST_CODE 0

int packet_sequence_number = 0;

struct s_icmp_echo_request_packet create_icmp_echo_request_packet()
{
    struct s_header header;
    struct s_icmp_echo_request_packet packet;
    
    header.type = ECHO_REQUEST_TYPE;
    header.code = 0;
    header.checksum = 0;
    header.identifier = getpid();

    packet.header = header;
    memset(&packet.payload, 13, sizeof(packet.payload));
    return packet;
}

unsigned short caclulate_checksum(void *b, int len) 
{    
    unsigned short *buf = b;
    unsigned int sum = 0;
    unsigned short result;

    for (sum = 0; len > 1; len -= 2)
        sum += *buf++;
    if (len == 1)
        sum += *(unsigned char *)buf;
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    result = ~sum;
    return result;
}

#define TIME_INTERVAL_BETWEEN_PINGS_SEC 1

int main(int ac, char **av) {
    if (ac != 2) {
        printf("Expect 1 argument.");
    }

    //--------------------------resolve host passed as an argument
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET; // Allow IPv4
    hints.ai_socktype = SOCK_RAW; // Raw socket for ICMP
    hints.ai_protocol = IPPROTO_ICMP; // ICMP protocol

    struct addrinfo *resloved_address;
    
    int result = getaddrinfo(av[1], NULL, &hints, &resloved_address);
    if (result != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(result)); //real ping prints: ping: host unknown in case of blabla host
        freeaddrinfo(resloved_address);
        exit(EXIT_ERROR_CODE);
    }


    //here i cast struct sockaddr* to struct sockaddr_in* because ai_addr may contain different address structures: struct sockaddr_in* - for ipv4, struct sockaddr_in6* for ipv6
    struct sockaddr_in remote_host_net_addr = *((struct sockaddr_in*) resloved_address->ai_addr);
    char remote_host_str_addr[INET_ADDRSTRLEN];
    bzero(remote_host_str_addr, INET_ADDRSTRLEN);
    
    if(inet_ntop(AF_INET,&(remote_host_net_addr.sin_addr), remote_host_str_addr, INET_ADDRSTRLEN) == NULL) {
        fprintf(stderr, "inet_ntop: %s\n", strerror(errno));
        freeaddrinfo(resloved_address);
        exit(EXIT_ERROR_CODE);
    }
    freeaddrinfo(resloved_address);
    
    //---------------------open socket
    int socket_fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (socket_fd == -1) {
        fprintf(stderr, "socket: %s\n", strerror(errno));
        exit(EXIT_ERROR_CODE);
    }
    
    //----------------------create packet;
    struct s_icmp_echo_request_packet packet = create_icmp_echo_request_packet();
    while (true)
    {
        sleep(TIME_INTERVAL_BETWEEN_PINGS_SEC);
        packet.header.sequence_number = packet_sequence_number++;
        packet.header.checksum = caclulate_checksum(&packet, PACKET_LENGTH);
        //-------------------- print ping header
        printf("PING %s (%s): %d data bytes\n", av[1], remote_host_str_addr, PING_PACKET_SIZE);
    }


    printf("Socket fd %d\n", socket_fd);
    return 0;
}
