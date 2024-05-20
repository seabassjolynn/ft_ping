#ifndef PING_SESSION_STAT_H
#define PING_SESSION_STAT_H

#include <sys/time.h> //struct timeval

#include "ping_session.h"

struct s_ping_session_stat
{
  int transmitted_packets_count;
  int received_packets_count;
  int lost_packets_percentage;
  float round_trip_min_ms;
  float round_trip_max_ms;
  float round_trip_average;
  float round_trips_stddev;
};

float calc_round_trip_time_ms(struct timeval *start, struct  timeval *end);

struct s_ping_session_stat calc_ping_session_stat(struct s_ping_session *ping_session);

void print_statistics(struct s_ping_session_stat *ping_session_stat);

#endif