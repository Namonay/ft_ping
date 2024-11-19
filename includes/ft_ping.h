#pragma once

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <sys/time.h>

#include <arpa/inet.h>
#include <netdb.h>

#define PACKET_SIZE 64

struct net_icmp_header
{
	uint8_t type;
	uint8_t code;
	uint16_t checksum;
	uint16_t id;
	uint16_t seq;
};

struct net_packet
{
	uint8_t					data[PACKET_SIZE];
	int						n_bytes;
	struct sockaddr_in		addr;
	struct net_icmp_header *icmp_hdr;
};

struct packet_stats
{
	uint16_t	n_packet_sent;
	uint16_t	n_packet_recv;
	double		timestamp_array[65536];
};

struct flags
{
	bool verbose;
	bool quiet;
	bool flood;
	double interval;
	int preload_count;
	int count;
	int ttl;
};

//	PACKET UTILS
uint16_t		make_checksum(uint16_t *data, int len);
struct in_addr	get_addr_by_hostname(char *hostname);
bool			parse_packet(struct net_packet packet, uint16_t seq, uint16_t checksum);

//	MATH
double			get_stddev(const double *timestamp_array);
double			get_avg(const double *timestamp_array);
double			get_min(const double *timestamp_array);
double			get_max(const double *timestamp_array);
long			ft_atoi(const char *nptr);

//	VERBOSE
void			print_help(void);
bool			parse_opt(int argc, char **argv, struct flags *flags);
void			print_packet_error(struct net_packet packet);

// STATS
void			print_recap(char *ip, const struct packet_stats stats);
void			fill_timestamp_array(struct packet_stats *sockstats, double time);
double			get_timestamp();