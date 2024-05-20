#ifndef NET_H
#define NET_H

#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>

#define SRC_ADDR_OFFSET_IN_IP_PACKET 12


void get_str_ip_addr_from_in_addr(struct in_addr *in_addr, char *str_ip_addr);

void get_srs_ip_addr_from_ip_packet_as_str(void *ip_packet, char *str_ip_addr);

int get_ip_header_length_in_bytes(uint8_t *packet);

uint16_t caclulate_checksum(void *addr, int len);

# define ECHO_PACKET_BODY_LENGTH 56 //this number is provided with -s flag. This is body length of echo packet. And this is what is written in ping header. 
//ECHO request length is then 56 + 8 (header length). String that describes received string writes number of bytes. And this number is length of ping reply packet 
//which is equal to length of echo request packet, and not IP packat
# define ECHO_PACKET_HEADER_LENGTH 8
# define ECHO_PACKET_LENGTH ECHO_PACKET_BODY_LENGTH + ECHO_PACKET_HEADER_LENGTH
// 60 bytes - maximum IP header length
// ICMP: Destination Unreachable Message - length is 8 (header) + ip header + 64 bits of packet data = 8 + 60 + 8 = 76 byte.
// the other types of ICMP messages are shorter

# define ECHO_REQUEST_TYPE 8

# define ECHO_REQUEST_CODE 0

# define ECHO_REPLY_TYPE 0

# define ECHO_REPLY_CODE 0
# include <stdint.h>
struct s_header
{
    uint8_t type;
    uint8_t code;
    uint16_t checksum;
    uint16_t identifier;
    uint16_t sequence_number;
};

struct s_icmp_echo_packet
{
    struct s_header header;
    unsigned char payload[ECHO_PACKET_BODY_LENGTH];
};

struct s_icmp_echo_packet create_icmp_echo_request_packet();

#endif