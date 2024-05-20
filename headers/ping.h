#ifndef PING_H
#define PING_H

#include "net.h"

struct s_ping_data ping(struct s_icmp_echo_packet echo_request);

#endif