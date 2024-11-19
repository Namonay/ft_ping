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
/* Returns the checksum of data*/
uint16_t		make_checksum(const uint16_t *data, int len);
/* Get the address of hostname from the host data base */
struct in_addr	get_addr_by_hostname(char *hostname);
bool			parse_packet(struct net_packet packet, uint16_t seq, uint16_t checksum);

//	MATH
/* Returns the standard deviation of timestamp_array*/
double			get_stddev(const double *timestamp_array);
/* Returns the average value of timestamp_array*/
double			get_avg(const double *timestamp_array);
/* Returns the smallest value of timestamp_array*/
double			get_min(const double *timestamp_array);
/* Returns the biggest value of timestamp_array*/
double			get_max(const double *timestamp_array);
long			ft_atoi(const char *nptr);

//	VERBOSE
bool			parse_opt(int argc, char **argv, struct flags *flags);
void			print_packet_error(struct net_packet packet);

// STATS
void			print_recap(char *ip, const struct packet_stats stats);
/* Add time into the timestamp_array in sockstats*/
void			fill_timestamp_array(struct packet_stats *sockstats, double time);
/* Returns the current time*/
double			get_timestamp();