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

//sleep
#include <unistd.h>

#include "net.h"
#include "ping_session.h"

#define IP_PACKET_BUFFER_LENGTH 1024

#include "debug.h"

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

bool is_pair(struct s_ping_header *request_ping_header, struct s_ping_header *reply_ping_header)
{
    return (request_ping_header->identifier == reply_ping_header->identifier) && (request_ping_header->sequence_number == reply_ping_header->sequence_number);
}

struct s_ping_data ping(struct s_icmp_echo_packet echo_request)
{
    struct s_icmp_echo_packet echo_request_copy = echo_request;
    
    struct s_ping_data ping_data;
    
    ping_data.time_start = get_time();
    ping_data.time_end.tv_sec = -1;
    ping_data.time_end.tv_usec = -1;
    
    int sendResult = sendto(g_resources.fd_socket, &echo_request_copy, ECHO_PACKET_LENGTH, 0, (g_resources.addr_info)->ai_addr, (g_resources.addr_info)->ai_addrlen);
    
    if (sendResult < 0) 
    {
        fprintf(stderr, "Error when sending ICPM packet. Error: %s\n", strerror(errno));
        free_resources();
        exit(EXIT_ERROR);
    }
    
    g_ping_session.sent_echo_count++;

    while(true)
    {   
        uint8_t received_ip_packet[IP_PACKET_BUFFER_LENGTH];
        bzero(received_ip_packet, IP_PACKET_BUFFER_LENGTH);
        ping_data.received_bytes_count = recvfrom(g_resources.fd_socket, received_ip_packet, IP_PACKET_BUFFER_LENGTH, 0, NULL, NULL);
        if (ping_data.received_bytes_count > 0)
        {
            debug_printf(g_ping_session.flags.is_debug, "Reply length: %ld\n", ping_data.received_bytes_count);
            
            int ip_header_length = get_ip_header_length_in_bytes(received_ip_packet);
            if (ip_header_length != sizeof(struct s_ip_header))
            {
                fprintf(stderr, "Receved ip packet of unexpected length: %d\n", ip_header_length);
                free_resources();
                exit(EXIT_ERROR);
            }
            
            ping_data.received_bytes_count = ping_data.received_bytes_count - ip_header_length;
            
            struct s_icmp_echo_packet *received_icmp_packet = (struct s_icmp_echo_packet *) (received_ip_packet + ip_header_length);
            if (is_icpm_reply(received_icmp_packet))
            {
                if (is_pair(&echo_request.header, &received_icmp_packet->header))
                {
                    ping_data.time_end = get_time();
                    ping_data.icmp_seq_number = echo_request.header.sequence_number;
                    ping_data.is_error_reply = false;
                    
                    bzero(ping_data.reply_host_str_addr, INET_ADDRSTRLEN);
                    get_srs_ip_addr_from_ip_packet_as_str(received_ip_packet, ping_data.reply_host_str_addr);
                    return ping_data;
                }
                else
                {
                    debug_printf(g_ping_session.flags.is_debug, "Received unexpected icpm reply. id / seq num of request: %d %d, id / seq num of reply: %d %d\n", echo_request.header.identifier, echo_request.header.sequence_number, received_icmp_packet->header.identifier, received_icmp_packet->header.sequence_number);
                }
                //continue - read packet again.
            }
            else if (is_ping_error_reply(received_icmp_packet->header.type))
            {
                debug_print(g_ping_session.flags.is_debug, "Received error ping reply\n");
                
                struct s_error_ping_reply *error_ping_reply = (struct s_error_ping_reply *) (received_ip_packet + ip_header_length);
                
                if (is_pair(&echo_request.header, &error_ping_reply->original_data.ping_header))
                {
                    ping_data.is_error_reply = true;
                    
                    bzero(ping_data.reply_host_str_addr, INET_ADDRSTRLEN);
                    get_srs_ip_addr_from_ip_packet_as_str(received_ip_packet, ping_data.reply_host_str_addr);
                    
                    bzero(ping_data.error_reply_code_description, ERROR_REPLY_CODE_DESCRIPTION_BUFFER_SIZE);
                    get_error_reply_code_description(error_ping_reply->type, error_ping_reply->code, ping_data.error_reply_code_description);
                    
                    bzero(&ping_data.error_reply, sizeof(struct s_error_ping_reply));
                    memcpy(&ping_data.error_reply, error_ping_reply, sizeof(struct s_error_ping_reply));
                    
                    return ping_data;
                }
                else
                {
                    debug_printf(g_ping_session.flags.is_debug, "Received unexpected icpm error reply. id / seq num of request: %d %d, id / seq num of reply: %d %d\n", echo_request.header.identifier, echo_request.header.sequence_number, error_ping_reply->original_data.ping_header.identifier, error_ping_reply->original_data.ping_header.sequence_number);
                }
            } 
            else 
            {
                debug_printf(g_ping_session.flags.is_debug, "Received unhandled type of ping. Type: %d\n", received_icmp_packet->header.code);
            }

        }
        else
        {
            // struct s_icmp_echo_packet r = *((struct s_icmp_echo_packet *) (receivedPacket + 20));
            // printf("Received ICMP reply. Id: %d sequence num: %d\n", r.header.identifier, r.header.sequence_number);
            debug_print(g_ping_session.flags.is_debug, "ping timeout\n");
            return ping_data;
        }
    }
}

// I still have probelm that I receive 76 bytes, But this is probably because my buffer is 76 bytes, but in wire shark I see that I receive 84 bytes,
// which makes sense 20 bytes - ip header and 64 bytes - ICPM reply length
