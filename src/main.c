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
    header.sequence_number = 0;
    
    packet.header = header;
    memset(&packet.payload, 'P', sizeof(packet.payload));
    return packet;
}

uint16_t caclulate_checksum(void *addr, int len) 
{    
    uint32_t sum = 0;
    while (len > 1)
    {
        sum += *((uint16_t *) addr);  
        addr += 2;
        len -= 2;
    }

    if( len > 0 )
    {
        sum += * (uint8_t*) addr;
    }
               

    /*  Fold 32-bit sum to 16 bits */
    while (sum >> 16)
    {
        sum = (sum & 0xffff) + (sum >> 16);
    }
    return ~sum;
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

    struct addrinfo *resolved_address;
    
    int result = getaddrinfo(av[1], NULL, &hints, &resolved_address);
    if (result != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(result)); //real ping prints: ping: host unknown in case of blabla host
        freeaddrinfo(resolved_address);
        exit(EXIT_ERROR_CODE);
    }


    //here i cast struct sockaddr* to struct sockaddr_in* because ai_addr may contain different address structures: struct sockaddr_in* - for ipv4, struct sockaddr_in6* for ipv6
    struct sockaddr_in remote_host_net_addr = *((struct sockaddr_in*) resolved_address->ai_addr);
    char remote_host_str_addr[INET_ADDRSTRLEN];
    bzero(remote_host_str_addr, INET_ADDRSTRLEN);
    
    if(inet_ntop(AF_INET,&(remote_host_net_addr.sin_addr), remote_host_str_addr, INET_ADDRSTRLEN) == NULL) {
        fprintf(stderr, "inet_ntop: %s\n", strerror(errno));
        freeaddrinfo(resolved_address);
        exit(EXIT_ERROR_CODE);
    }
    freeaddrinfo(resolved_address);
    
    //---------------------open socket
    int socket_fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (socket_fd == -1) {
        fprintf(stderr, "socket: %s\n", strerror(errno));
        exit(EXIT_ERROR_CODE);
    }
    
    struct timeval tv_out;
    tv_out.tv_sec = 1;
    tv_out.tv_usec = 0;
    setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv_out, sizeof tv_out);

    //----------------------create packet;
    int sequence_number = 0;
    while (true)
    {
        sleep(TIME_INTERVAL_BETWEEN_PINGS_SEC);
        struct s_icmp_echo_request_packet packet = create_icmp_echo_request_packet();
        packet.header.sequence_number = sequence_number++;
        packet.header.checksum = caclulate_checksum(&packet, PACKET_LENGTH);
        
        //-------------------- print ping header
        printf("PING %s (%s): %d data bytes\n", av[1], remote_host_str_addr, PING_PACKET_SIZE);
        int sendResult = sendto(socket_fd, &packet, PACKET_LENGTH, 0, resolved_address->ai_addr, resolved_address->ai_addrlen);
        if (sendResult == -1) 
        {
            printf("Error when sending ICPM packet. Error: %s\n", strerror(errno));
            //need cleanup function to free address and socket
            exit(EXIT_ERROR_CODE);
        } else {
            printf("Send ICPM packet successfuly. Number of bytes sent is: %d\n", sendResult);
        }
        
        uint8_t receivedPacket[400];
        bzero(receivedPacket, 400);
        int recieveResult = recvfrom(socket_fd, receivedPacket, 400, 0, NULL, 0);
        if (recieveResult != -1)
        {
            //printf("Received result: %d\n", recieveResult);
            //int ip_header_len = receivedPacket[0] & 0xf;
            //printf("Ip header length: %d\n", ip_header_len);
            struct s_icmp_echo_request_packet r = *((struct s_icmp_echo_request_packet *) (receivedPacket + 20));
            printf("Received ICMP reply. Id: %d sequence num: %d\n", r.header.identifier, r.header.sequence_number);
        }
        else
        {
            printf("Failed to receive ICPM packet");
        }
    }

    printf("Socket fd %d\n", socket_fd);
    //close socket?
    //freeaddrinfo(resolved_address);
    return 0;
}
