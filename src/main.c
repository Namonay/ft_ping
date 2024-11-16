#include "ft_ping.h"

struct packet_stats	stats;
bool				loop = true;

static int ft_ping(const int sock, const uint16_t seq, const struct sockaddr_in *dst)
{
	unsigned char		data[PACKET_SIZE];
	struct icmp_header	*icmp_hdr = (struct icmp_header *)data;

	memset(data, 0, sizeof(data));

	icmp_hdr->type = ICMP_ECHO;
	icmp_hdr->code = 0;
	icmp_hdr->id = getpid();
	icmp_hdr->seq = seq;
	icmp_hdr->checksum = make_checksum((uint16_t *)icmp_hdr, sizeof(icmp_hdr));

	if (sendto(sock, data, sizeof(data), 0, (struct sockaddr *)dst, sizeof(struct sockaddr_in)) == -1)
	{
		fprintf(stderr, "ERROR : Network is unreachable\n");
		return (0);
	}
	stats.n_packet_sent++;
	return (1);
}

static int ft_recv(const int sock, const uint16_t seq, const double start, const bool quiet)
{
	unsigned char		data[PACKET_SIZE];
	struct icmp_header *icmp_hdr = (struct icmp_header *)(data + 20);
	int					n_bytes;
	struct sockaddr_in	addr;
	int					len = sizeof(addr);
	double				time;
	uint16_t			checksum;

	memset(data, 0, sizeof(data));
	n_bytes = recvfrom(sock, data, sizeof(data), 0, (struct sockaddr *)&addr, (socklen_t *)&len);
	if (n_bytes < 1)
		return (1);
	time = (get_timestamp() - start) * 1000;
	checksum = icmp_hdr->checksum;
	icmp_hdr->checksum = 0;
	if (icmp_hdr->seq != seq || make_checksum((uint16_t *)icmp_hdr, sizeof(*icmp_hdr)) != checksum)
		return (0);
	fill_timestamp_array(&stats, time);
	stats.n_packet_recv++;
	if (!quiet)
		printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%5.3fms\n", n_bytes, inet_ntoa(addr.sin_addr), icmp_hdr->seq, (uint8_t)data[8], time);
	return (1);
}

static void signal_handler(int code)
{
	(void)code;
	loop = false;
}

static void init_signal()
{
	signal(SIGINT, signal_handler);
}

static bool init_socket(int *sock, struct sockaddr_in *dst, char *host)
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

int main(int argc, char **argv)
{
	int					socket_fd;
	struct sockaddr_in	dst_addr;
	char				*ip;
	uint16_t			seq = 1;
	double				start;
	struct flags		flags = {false, false, false, 1, -1, -1};

	if (parse_opt(argc, argv, &flags) == false)
		return (0);
	init_signal();
	if (!init_socket(&socket_fd, &dst_addr, argv[optind]))
		return (-1);
	ip = inet_ntoa(get_addr_by_hostname(argv[optind]));
	if (flags.verbose)
		printf("PING %s (%s) : %d data bytes, id 0x%04x = %d\n", argv[optind], ip, (PACKET_SIZE - 8), getpid(), getpid());
	else
		printf("PING %s (%s) : %d data bytes\n", argv[optind], ip, (PACKET_SIZE - 8));
	while (loop)
	{
		start = get_timestamp();
		if (ft_ping(socket_fd, seq, &dst_addr) == 0)
			break;
		while (!ft_recv(socket_fd, seq, start, flags.quiet));
		seq++;
		if (flags.count != -1)
			flags.count--;
		if (flags.count == 0)
			break;
		if (flags.preload_count <= 0)
			usleep(flags.interval * 1000000);
		else
			flags.preload_count--;
	}
	print_recap(argv[optind], stats);
	close(socket_fd);
	return (0);
}
