#ifndef ICPM_PACKET_H
# define ICPM_PACKET_H
# define PACKET_LENGTH 56

struct s_header
{
    unsigned char type;
    unsigned char code;
    short checksum;
    short identifier;
    short sequence_number;
};

struct s_icmp_echo_request_packet
{
    struct s_header header;
    unsigned char payload[PACKET_LENGTH - sizeof(struct s_header)];
};

#endif