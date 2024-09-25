#include "ft_ping.h"

struct packet_stats	stats;
bool				loop = true;

int ft_ping(int sock, uint16_t seq, struct sockaddr_in *dst)
{
	unsigned char		data[64];
	struct icmp_header	*icmp_hdr = (struct icmp_header *)data;

	memset(data, 0, sizeof(data));

	icmp_hdr->type = ICMP_ECHO;
	icmp_hdr->code = 0;
	icmp_hdr->id = getpid();
	icmp_hdr->seq = seq;
	icmp_hdr->checksum = calculate_checksum((uint16_t *)icmp_hdr, sizeof(icmp_hdr));

	if (sendto(sock, data, sizeof(data), 0, (struct sockaddr *)dst, sizeof(struct sockaddr_in)) == -1)
	{
		fprintf(stderr, "ERROR : Network is unreachable\n");
		return (0);
	}
	stats.n_packet_sent++;
	return (1);
}

void ft_recv(int sock, uint16_t seq, char *ip, double start)
{
	unsigned char		data[64];
	struct icmp_header *icmp_hdr = (struct icmp_header *)(data + 20);
	int					n_bytes;
	struct sockaddr_in	addr;
	int					len = sizeof(addr);
	double				time;
	uint16_t			checksum;

	memset(data, 0, sizeof(data));
	n_bytes = recvfrom(sock, data, sizeof(data), 0, (struct sockaddr *)&addr, (socklen_t *)&len);
	if (n_bytes < 1)
		return;
	time = (get_timestamp() - start) * 1000;
	checksum = icmp_hdr->checksum;
	icmp_hdr->checksum = 0;
	if (icmp_hdr->seq != seq || calculate_checksum((uint16_t *)icmp_hdr, sizeof(*icmp_hdr)) != checksum)
		return;
	fill_timestamp_array(&stats, time);
	stats.n_packet_recv++;
	printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%5.3fms\n", n_bytes, ip, icmp_hdr->seq, (uint8_t)data[8], time);
	return;
}

void handler(int code)
{
	(void)code;
	loop = 0;
}

void init_signal()
{
	signal(SIGINT, handler);
}

bool init_socket(int *sock, struct sockaddr_in *dst, char *host)
{
	struct timeval timeout;

	memset(dst, 0, sizeof(*dst));
	memset(&stats.timestamp_array, 0, sizeof(stats.timestamp_array));

	dst->sin_family = AF_INET;
	dst->sin_port = 0;
	if (gethostbyname(host) == NULL && inet_aton(host, &dst->sin_addr) == 0)
	{
		fprintf(stderr, "ERROR : %s is an unknown host\n", host);
		return (false);
	}
	dst->sin_addr = get_addr_by_hostname(host);
	if ((*sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0) // create a RAW socket for issuing ICMP Requests
	{
		fprintf(stderr, "ERROR : socket() failed, are you sudo ?\n"); // creating a RAW socket will fail if we are not superuser
		return (false);
	}
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;
	setsockopt(*sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)); // use the timeval struct to set a timeout to our socket
	return (true);
}

void print_recap(char *ip)
{
	printf("--- %s ping statistics ---\n", ip);
	printf("%d packed transmitted, %d received, %0.0f%% packet loss\n", stats.n_packet_sent, stats.n_packet_recv, (double)(100 - (stats.n_packet_recv / stats.n_packet_sent) * 100));
	printf("round-trip min/avg/max/stddev = %5.3f/%5.3f/%5.3f/%5.3f ms\n", get_min(stats.timestamp_array), get_avg(stats.timestamp_array), get_max(stats.timestamp_array), get_stddev(stats.timestamp_array));
}
int main(int argc, char **argv)
{
	int					sock;
	struct sockaddr_in	dst;
	char				*ip;
	uint16_t			seq = 1;
	double				start;
	int					flags;
	bool				verbose = false;

	while ((flags = getopt(argc, argv, "v?")) != -1)
	{
		switch (flags)
		{
			case 'v':
				verbose = true;
				break;
			case '?':
				fprintf(stdout, "usage : %s {-v?} [ADRESS]\n", argv[0]);
				return (1);
		}
	}
	if (argc < 2 || argv[optind] == NULL || argv[optind][0] == 0)
	{
		fprintf(stderr, "ERROR : usage : %s {-v?} [ADRESS]\n", argv[0]);
		return (0);
	}
	init_signal();
	if (!init_socket(&sock, &dst, argv[optind]))
		return (-1);
	ip = inet_ntoa(get_addr_by_hostname(argv[optind]));
	if (verbose)
		fprintf(stdout, "PING %s (%s) : %d data bytes, id 0x%04x = %d\n", argv[optind], ip, PACKET_SIZE, getpid(), getpid());
	else
		fprintf(stdout, "PING %s (%s) : %d data bytes\n", argv[optind], ip, PACKET_SIZE);
	while (loop)
	{
		start = get_timestamp();
		if (ft_ping(sock, seq, &dst) == 0)
			break;
		ft_recv(sock, seq, ip, start);
		seq++;
		sleep(1);
	}
	print_recap(argv[optind]);
	close(sock);
	return (0);
}
