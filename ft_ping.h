#pragma once

#include <inttypes.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <errno.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <netdb.h>

struct t_socket
{
	struct sockaddr_in dst, src;
	int fd;
	int len;
};