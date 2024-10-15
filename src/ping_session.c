#include "ping_session.h"

//NULL definition
#include "stdlib.h"
//getpid()
#include <unistd.h>

#include "exit_constants.h"

//prinf
#include <stdio.h>
#include "resources.h"
#include <string.h> //memcpy
#include "debug.h"
#include "ping_session_stat.h" //calc_round_trip_time_ms

struct s_ping_session g_ping_session;

void print_ping_header()
{
    printf("PING %s (%s): %d data bytes", g_ping_session.target_arg, g_ping_session.target_addr, ECHO_PACKET_BODY_LENGTH);
    
    if (g_ping_session.flags.is_verbose)
    {
        printf(", id 0x%x = %d", g_ping_session.id, g_ping_session.id);
    }

    printf("\n");
}

void print_icmp_echo_reply(struct s_ping_data *ping_data)
{
    float time = calc_round_trip_time_ms(&ping_data->time_start, &ping_data->time_end);

    printf("%ld bytes from %s: icmp_seq=%ld ttl=%d time=%.3f\n", ping_data->received_bytes_count, ping_data->reply_host_str_addr, ping_data->icmp_seq_number, g_ping_session.flags.ttl, time);
}

static void print_original_ip_header_hex_dump(struct s_ip_header *ip_header)
{
    printf("IP Hdr Dump:\n");
    
    int ip_header_length_in_2_bytes_chuncks = sizeof(struct s_ip_header) / 2;
    uint16_t *ip_header_uint16_ptr = (uint16_t *)ip_header;
    
    int i = 0;
    while (i < ip_header_length_in_2_bytes_chuncks)
    {
        printf("%04x ", ntohs(*ip_header_uint16_ptr));
        ip_header_uint16_ptr++;
        i++;
    }
    printf("\n");
}

static int get_ecn(struct s_ip_header *ip_header)
{
    return ip_header->tos & 0b00000011;
}

static int get_dscp(struct s_ip_header *ip_header)
{
    return (ip_header->tos & 0b11111100) >> 2;
}

static int get_ihl(struct s_ip_header *ip_header)
{
    return (ip_header->version_and_ihl) & 0x0f;
}

static int get_ip_version(struct s_ip_header *ip_header)
{
    return (ip_header->version_and_ihl & 0xf0) >> 4;
}

static void print_original_ip_header(struct s_ip_header *ip_header) 
{
    printf("Vr HL TOS  Len   ID Flg  off TTL Pro  cks      Src      Dst     Data\n");
        
    int ip_version = get_ip_version(ip_header);
    int ihl = get_ihl(ip_header);
    int ecn = get_ecn(ip_header);
    int dscp = get_dscp(ip_header);
    int total_length = ntohs(ip_header->total_length);
    int id = ntohs(ip_header->identification);
    int flg = (ntohs(ip_header->flags_and_fragment_offset) & 0b1110000000000000) >> 13;
    int fragment_offset = (ntohs(ip_header->flags_and_fragment_offset) & 0b0001111111111111);
    int ttl = (ip_header->ttl); //no need to do ntohs as it is one byte long
    int protocol = ip_header->protocol; //no need to do ntohs as it is one byte long
    uint16_t checksum = ntohs(ip_header->header_checksum);
    
    char src_addr[INET_ADDRSTRLEN];
    int_addr_to_ipv4_addr_str(ip_header->src_address, src_addr); 
    
    char dst_addr[INET6_ADDRSTRLEN];
    int_addr_to_ipv4_addr_str(ip_header->dst_address, dst_addr);

    printf(" %d  ", ip_version);
    printf("%d  ", ihl);
    printf("%d", dscp);
    printf("%d ", ecn);
    printf("%04x ", total_length);
    printf("%04x   ", id);
    printf("%d ", flg);
    printf("%04x  ", fragment_offset);
    printf("%02x  ", ttl);
    printf("%02x ", protocol);
    printf("%04x ", checksum);
    printf("%s  ", src_addr);
    printf("%s\n", dst_addr);
}

static void print_original_icpm(struct s_ping_header *ping_header) 
{
    printf("ICMP: type %d, ", ping_header->type);
    printf("code %d, ", ping_header->code);
    printf("size %u, ", sizeof(struct s_icmp_echo_packet));
    printf("id 0x%04x, ", ping_header->identifier);
    printf("seq 0x%04x\n", ping_header->sequence_number);
}

void print_icmp_error_reply(struct s_ping_data *ping_data)
{
    printf("%ld bytes from %s: %s\n", ping_data->received_bytes_count, ping_data->reply_host_str_addr, ping_data->error_reply_code_description);
    if (g_ping_session.flags.is_verbose)
    {
        print_original_ip_header_hex_dump(&ping_data->error_reply.original_data.ip_header);
        print_original_ip_header(&ping_data->error_reply.original_data.ip_header);
        print_original_icpm(&ping_data->error_reply.original_data.ping_header);
    }
}

void add_to_ping_session(struct s_ping_data *ping_data)
{
    debug_printf("add_to_ping_session. Ping data storage capacity: %d, index: %d\n", g_ping_session.ping_data_arr_count, g_ping_session.ping_data_arr_next_index);
    
    if (g_ping_session.ping_data_arr_next_index == g_ping_session.ping_data_arr_count)
    {
        g_ping_session.ping_data_arr_count += 10;
        
        debug_printf("New ping data storage capacity: %d\n", g_ping_session.ping_data_arr_count);

        g_ping_session.ping_data_arr = realloc(g_ping_session.ping_data_arr, sizeof(struct s_ping_data) * g_ping_session.ping_data_arr_count);
        if (g_ping_session.ping_data_arr == NULL)
        {
            free_resources();
            exit(EXIT_ERROR);   
        }
        g_resources.ping_data_arr = g_ping_session.ping_data_arr;
    }
    
    memcpy(g_ping_session.ping_data_arr + g_ping_session.ping_data_arr_next_index, ping_data, sizeof(struct s_ping_data));
    g_ping_session.ping_data_arr_next_index++;
}

void init()
{
    g_ping_session.flags.ttl = 63;
    g_ping_session.flags.is_verbose = false;
    g_ping_session.flags.print_man_only = false;
    g_ping_session.flags.is_debug = false;
    g_ping_session.flags.linger.tv_sec = 1;
    g_ping_session.flags.linger.tv_usec = 0;
    g_ping_session.flags.count = -1;
    g_ping_session.flags.timeout = -1;

    bzero(g_ping_session.target_addr, INET_ADDRSTRLEN);
    g_ping_session.ping_data_arr_count = 0;
    g_ping_session.ping_data_arr_next_index = 0;
    g_ping_session.flags.interval_between_pings_usec = 1000000;
    
    g_ping_session.id = getpid();
}


















