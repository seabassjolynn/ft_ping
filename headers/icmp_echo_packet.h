#ifndef ICPM_PACKET_H
# define ICPM_PACKET_H
# define PACKET_LENGTH 56

struct s_header
{
    uint8_t type;
    uint8_t code;
    uint16_t checksum;
    uint16_t identifier;
    uint16_t sequence_number;
};

struct s_icmp_echo_request_packet
{
    struct s_header header;
    unsigned char payload[PACKET_LENGTH - sizeof(struct s_header)];
};

#endif