#include <sys/time.h>
#include "ping_session.h"
#include "resources.h"

//errno
#include <errno.h>

#include <stdio.h>
#include "exit_constants.h"
#include <stdbool.h>

//exit
#include <stdlib.h>

//memeset
#include <string.h>

#include "net.h"
#include "ping_session.h"

#define IP_PACKET_BUFFER_LENGTH 1024

//gettimeof a day
struct timeval get_time()
{
    struct timeval time;
    int get_time_result = gettimeofday(&time, NULL);
    if (get_time_result == -1)
    {
        fprintf(stderr, "Error when getting time stamp for ping. Error: %s\n", strerror(errno));
        free_resources();
        exit(EXIT_ERROR);
    }
    return time;
}

bool is_icpm_reply(struct s_icmp_echo_packet *icmp_packet_ptr)
{
    return icmp_packet_ptr->header.type == ECHO_REPLY_TYPE && icmp_packet_ptr->header.code == ECHO_REPLY_CODE;
}

bool is_pair(struct s_icmp_echo_packet *request, struct s_icmp_echo_packet *reply)
{
    return (request->header.identifier == reply->header.identifier) && (request->header.sequence_number == reply->header.sequence_number);
}

struct s_ping_data ping(struct s_icmp_echo_packet echo_request)
{
    struct s_icmp_echo_packet echo_request_copy = echo_request;
    
    struct s_ping_data ping_data;
    
    ping_data.icmp_seq_number = echo_request.header.sequence_number;
    ping_data.time_start = get_time();
    
    int sendResult = sendto(g_resources.fd_socket, &echo_request_copy, ECHO_PACKET_LENGTH, 0, (g_resources.addr_info)->ai_addr, (g_resources.addr_info)->ai_addrlen);
    
    if (sendResult < 0) 
    {
        fprintf(stderr, "Error when sending ICPM packet. Error: %s\n", strerror(errno));
        free_resources();
        exit(EXIT_ERROR);
    }
    
    
    while(true)
    {   
        uint8_t received_ip_packet[IP_PACKET_BUFFER_LENGTH];
        bzero(received_ip_packet, IP_PACKET_BUFFER_LENGTH);
        
        ping_data.received_bytes_count = recvfrom(g_resources.fd_socket, received_ip_packet, IP_PACKET_BUFFER_LENGTH, 0, NULL, NULL);
        if (ping_data.received_bytes_count > 0)
        {
            int ip_header_length = get_ip_header_length_in_bytes(received_ip_packet);
            ping_data.received_bytes_count = ping_data.received_bytes_count - ip_header_length;
            
            struct s_icmp_echo_packet *received_icmp_packet = (struct s_icmp_echo_packet *) (received_ip_packet + ip_header_length);
            if (is_icpm_reply(received_icmp_packet))
            {
                if (is_pair(&echo_request, received_icmp_packet))
                {
                    bzero(ping_data.reply_host_str_addr, INET_ADDRSTRLEN);
                    get_srs_ip_addr_from_ip_packet_as_str(received_ip_packet, ping_data.reply_host_str_addr);
                
                    ping_data.time_end = get_time();
                    return ping_data;
                }
                else
                {
                    printf("Not pair. id / seq request: %d %d id / seq reply: %d %d\n", echo_request.header.identifier, echo_request.header.sequence_number, received_icmp_packet->header.identifier, received_icmp_packet->header.sequence_number);
                }
                //continue - read packet again.
            }
            // else
            // {
            //     //handle error reply
            //     if (my_error_reply)
            //     {
            //         print error when -v flag.
            //     }
            //     //this was not my packet, so i will try to read again, so continue
            // }
        }
        else
        {
            // struct s_icmp_echo_packet r = *((struct s_icmp_echo_packet *) (receivedPacket + 20));
            // printf("Received ICMP reply. Id: %d sequence num: %d\n", r.header.identifier, r.header.sequence_number);
            return ping_data;
        }
    }
}

// I still have probelm that I receive 76 bytes, But this is probably because my buffer is 76 bytes, but in wire shark I see that I receive 84 bytes,
// which makes sense 20 bytes - ip header and 64 bytes - ICPM reply length