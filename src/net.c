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
#include "ping_session.h"

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

void ipv4_network_to_str(uint32_t addr, char *str_addr) {
    struct in_addr in_addr;
    in_addr.s_addr = addr;
    bzero(str_addr, INET_ADDRSTRLEN);
    
    if(inet_ntop(AF_INET,&in_addr, str_addr, INET_ADDRSTRLEN) == NULL) {
        fprintf(stderr, "inet_ntop: %s\n", strerror(errno));
        free_resources();
        exit(EXIT_ERROR);
    }
}

int get_ip_header_length_in_bytes(uint8_t *packet)
{
    return (packet[0] & 0XF) * 4; //ip header length value is a length measured in 32 bit words, so we multiply for 4 to find the length in bytes. 
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
    struct s_ping_header header;
    struct s_icmp_echo_packet packet;
    
    header.type = ECHO_REQUEST_TYPE;
    header.code = 0;
    header.checksum = 0;
    header.sequence_number = -1;
    header.identifier = g_ping_session.id;
    
    packet.header = header;
    memset(&packet.payload, 'A', sizeof(packet.payload));
    return packet;
}

bool is_ping_error_reply(int code)
{
    int types[5] = {3, 4, 5, 11, 12};
    int i = 0;
    while (i < 5)
    {
        if (types[i] == code)
        {
            return true;
        }
        i++;
    }
    return false;
}

void get_error_reply_code_description(int type, int code, char *description)
{
    switch (type) {
        case 3: // Destination Unreachable (RFC 792)
            switch (code) {
                case 0:
                    strcpy(description, "Destination Net Unreachable");
                    break;
                case 1:
                    strcpy(description, "Destination Host Unreachable");
                    break;
                case 2:
                    strcpy(description, "Protocol Unreachable");
                    break;
                case 3:
                    strcpy(description, "Port Unreachable");
                    break;
                case 4:
                    strcpy(description, "Fragmentation Needed and Don't Fragment was Set");
                    break;
                case 5:
                    strcpy(description, "Source Route Failed");
                    break;
                 default:
                    strcpy(description, "Unknown Code");
                    break;
            }
            break;
        case 4: // Source Quench (Deprecated, RFC 6633)
            strcpy(description, "Source Quench");
            break;
        case 5: // Redirect (RFC 792)
            switch (code) {
                case 0:
                    strcpy(description, "Redirect Datagram for the Network");
                    break;
                case 1:
                    strcpy(description, "Redirect Datagram for the Host");
                    break;
                case 2:
                    strcpy(description, "Redirect Datagram for the Type of Service and Network");
                    break;
                case 3:
                    strcpy(description, "Redirect Datagram for the Type of Service and Host");
                    break;
                default:
                    strcpy(description, "Unknown Code");
                    break;
            }
            break;
        case 11: // Time Exceeded (RFC 792)
            switch (code) {
                case 0:
                    strcpy(description, "Time to live exceeded");
                    break;
                case 1:
                    strcpy(description, "Fragment reassembly time exceeded");
                    break;
                default:
                    strcpy(description, "Unknown Code");
                    break;
            }
            break;
        case 12: // Parameter Problem (RFC 792)
            switch (code) {
                case 0:
                    strcpy(description, "Pointer indicates the error");
                    break;
                default:
                    strcpy(description, "Unknown Code");
                    break;
            }
            break;
        default:
            strcpy(description, "Unknown Type");
            break;
    }
}
