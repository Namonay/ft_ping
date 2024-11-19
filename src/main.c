#include "ft_ping.h"

struct packet_stats	stats;
bool				loop = true;

static int ft_ping(const int sock, const uint16_t seq, const struct sockaddr_in *dst)
{
	struct net_packet packet;

	memset(packet.data, 0, sizeof(packet.data));

	packet.icmp_hdr = (struct net_icmp_header *)packet.data;
	packet.icmp_hdr->type = 8; // ICMP_ECHO
	packet.icmp_hdr->code = 0; // Always 0 for a ICMP_ECHO
	packet.icmp_hdr->id = getpid();
	packet.icmp_hdr->seq = seq;
	packet.icmp_hdr->checksum = make_checksum((uint16_t *)packet.icmp_hdr, sizeof(packet.icmp_hdr));

	if (sendto(sock, packet.data, sizeof(packet.data), 0, (struct sockaddr *)dst, sizeof(struct sockaddr_in)) == -1)
	{
		fprintf(stderr, "error : sending packet : Network is unreachable\n");
		return (0);
	}
	#ifdef DEBUG
		printf("\e[1;31m[DEBUG]\e[1;00m sendto() packet header: type:%d code:%d checksum:%x id:%d icmp_seq:%d\n", packet.icmp_hdr->type, packet.icmp_hdr->code, packet.icmp_hdr->checksum, packet.icmp_hdr->id, packet.icmp_hdr->seq);
	#endif
	stats.n_packet_sent++;
	return (1);
}

static int ft_recv(const int sock, const uint16_t seq, const double start, const bool quiet)
{
	struct net_packet		packet;
	int						len = sizeof(packet.addr);
	double					time;
	uint16_t				checksum;

	packet.icmp_hdr = (struct net_icmp_header *)(packet.data + 20); // get the packet icmp_header (starts at 20 bytes)
	memset(packet.data, 0, sizeof(packet.data));
	packet.n_bytes = recvfrom(sock, packet.data, sizeof(packet.data), 0, (struct sockaddr *)&packet.addr, (socklen_t *)&len);
	if (packet.n_bytes < 1)
		return (1);
	time = (get_timestamp() - start) * 1000;
	checksum = packet.icmp_hdr->checksum;
	packet.icmp_hdr->checksum = 0;
	if (parse_packet(packet, seq, checksum) == false)
	{
		print_packet_error(packet);
		return (1);
	}
	fill_timestamp_array(&stats, time);
	stats.n_packet_recv++;
	if (!quiet)
		printf("%d bytes from %s: icmp_seq=%d ttl=%d time=%5.3fms\n", packet.n_bytes, inet_ntoa(packet.addr.sin_addr), packet.icmp_hdr->seq, (uint8_t)packet.data[8], time);
	#ifdef DEBUG
		printf("\e[1;31m[DEBUG]\e[1;00m recv() packet header: type:%d code:%d checksum:%x id:%d icmp_seq:%d\n", packet.icmp_hdr->type, packet.icmp_hdr->code, checksum, packet.icmp_hdr->id, packet.icmp_hdr->seq);
	#endif
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

static bool init_socket(int *sock, struct sockaddr_in *dst, char *host, int ttl)
{
	struct timeval timeout;

	memset(dst, 0, sizeof(*dst));
	memset(&stats.timestamp_array, 0, sizeof(stats.timestamp_array));

	dst->sin_family = AF_INET;
	dst->sin_port = 0;
	if (gethostbyname(host) == NULL && inet_aton(host, &dst->sin_addr) == 0) // Resolve the ip of the host and check if the ip is valid
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
	if (ttl != -1)
		setsockopt(*sock, IPPROTO_IP, IP_TTL, &ttl, sizeof(ttl)); // set the ttl (Time To Live) value if the parameter is given
	return (true);
}

int main(int argc, char **argv)
{
	int					socket_fd;
	struct sockaddr_in	dst_addr;
	char				*ip;
	uint16_t			seq = 1;
	double				start;
	struct flags		flags = {false, false, false, 1, -1, -1, -1};

	if (parse_opt(argc, argv, &flags) == false)
		return (0);
	if (!init_socket(&socket_fd, &dst_addr, argv[optind], flags.ttl))
		return (-1);
	ip = inet_ntoa(get_addr_by_hostname(argv[optind])); // get the ip as a string
	if (flags.verbose)
		printf("PING %s (%s) : %d data bytes, id 0x%04x = %d\n", argv[optind], ip, (PACKET_SIZE - 8), getpid(), getpid());
	else
		printf("PING %s (%s) : %d data bytes\n", argv[optind], ip, (PACKET_SIZE - 8));
	init_signal();
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
		if (flags.preload_count > 0)
			flags.preload_count--;
		else
			usleep(flags.interval * 1000000);
	}
	print_recap(argv[optind], stats);
	close(socket_fd);
	return (0);
}
