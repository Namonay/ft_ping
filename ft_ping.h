#pragma once

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>

#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <netdb.h>

struct icmp_header
{
	uint8_t type;
	uint8_t code;
	uint16_t checksum;
	uint16_t id;
	uint16_t seq;
};
