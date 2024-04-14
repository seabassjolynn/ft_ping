#include <netdb.h>
#include <stdio.h>
#include <errno.h>
#include <string.h> //bzero
#include <arpa/inet.h> //inet_ntop
#include <stdlib.h> //exit

#include "exit_constants.h"
#include "net.h"
#include "resources.h"

#include <unistd.h> //getpid()

//here i cast struct sockaddr* to struct sockaddr_in* because ai_addr may contain different address structures: struct sockaddr_in* - for ipv4, struct sockaddr_in6* for ipv6
void get_str_ip_addr_from_in_addr(struct in_addr *in_addr, char *str_ip_addr)
{
    if(inet_ntop(AF_INET,in_addr, str_ip_addr, INET_ADDRSTRLEN) == NULL) {
        fprintf(stderr, "inet_ntop: %s\n", strerror(errno));
        free_resources();
        exit(EXIT_ERROR);
    }
}

void get_srs_ip_addr_from_ip_packet_as_str(void *ip_packet, char *str_ip_addr) 
{
    uint32_t addr = *((uint32_t *)(ip_packet + SRC_ADDR_OFFSET_IN_IP_PACKET));
    struct in_addr in_addr;
    in_addr.s_addr = addr;
    get_str_ip_addr_from_in_addr(&in_addr, str_ip_addr);
}

int get_ip_header_length_in_bytes(uint8_t *packet)
{
    return (packet[0] & 0XF) * 4; //ip header length value is a length measured in 32 bit words, so we multiply on 4 to find the length. 
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

struct s_icmp_echo_packet create_icmp_echo_request_packet()
{
    struct s_header header;
    struct s_icmp_echo_packet packet;
    
    header.type = ECHO_REQUEST_TYPE;
    header.code = 0;
    header.checksum = 0;
    header.sequence_number = -1;
    header.identifier = getpid();
    
    packet.header = header;
    memset(&packet.payload, 'A', sizeof(packet.payload));
    return packet;
}