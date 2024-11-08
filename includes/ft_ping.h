#pragma once


#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include <math.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>

#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>

#define PACKET_SIZE 56

struct icmp_header
{
	uint8_t type;
	uint8_t code;
	uint16_t checksum;
	uint16_t id;
	uint16_t seq;
};

struct packet_stats
{
	uint16_t	n_packet_sent;
	uint16_t	n_packet_recv;
	double		timestamp_array[65536];
};

uint16_t make_checksum(uint16_t *data, int len);
double get_timestamp();
struct in_addr get_addr_by_hostname(char *hostname);
double get_stddev(double *timestamp_array);
void fill_timestamp_array(struct packet_stats *sockstats, double time);
double get_avg(double *timestamp_array);
double get_min(double *timestamp_array);
double get_max(double *timestamp_array);
