#include "ft_ping.h"

int					n_packet_sent = 0;
int					n_packet_recv = 0;
char				*ip;
char				loop = 1;

uint16_t calculate_checksum(uint16_t *data, int len)
{
	uint32_t checksum = 0;

	while (len > 1)
	{
		checksum += *data++;
		len -= 2;
	}
	if (len == 1)
		checksum += *(uint8_t *)data;

	while (checksum >> 16)
		checksum = (checksum & 0xFFFF) + (checksum >> 16);
	return (~checksum);
}

double get_timestamp()
{
	struct timeval timestamp;

	gettimeofday(&timestamp, NULL);
	return (timestamp.tv_sec + (double)timestamp.tv_usec / 1000000);
}

int ft_ping(int sock, int seq, struct sockaddr_in dst)
{
	unsigned char		data[64];
	struct icmp_header	*icmp_hdr = (struct icmp_header *)data;

	memset(data, 0, sizeof(data));
	memset(icmp_hdr, 0, sizeof(*icmp_hdr));

	icmp_hdr->type = ICMP_ECHO;
	icmp_hdr->code = 0;
	icmp_hdr->id = getpid();
	icmp_hdr->seq = seq;
	icmp_hdr->checksum = calculate_checksum((uint16_t *)icmp_hdr, sizeof(icmp_hdr));

	if (sendto(sock, data, sizeof(data), 0, (struct sockaddr *)&dst, sizeof(dst)) == -1)
	{
		fprintf(stderr, "ERROR : sendto() failed\n");
		return (0);
	}
	return (1);
}

int ft_recv(int sock, int seq, char *ip, double start)
{
	unsigned char		data[64];
	struct icmp_header *icmp_hdr = (struct icmp_header *)(data + 20);
	int					n_bytes;
	struct sockaddr_in	addr;
	int					len = sizeof(addr);
	double				time;

	n_bytes = recvfrom(sock, data, sizeof(data), 0, (struct sockaddr *)&addr, (socklen_t *)&len);
	while (icmp_hdr->type != 0 && n_bytes > 0)
		n_bytes = recvfrom(sock, data, sizeof(data), 0, (struct sockaddr *)&addr, (socklen_t *)&len);
	time = (get_timestamp() - start) * 1000000;
	if (icmp_hdr->seq != seq || calculate_checksum((uint16_t *)data, sizeof(data)))
		return (-1);
	printf("%d bytes from %s: icmp_seq:%d time:%5.3fms\n", n_bytes, ip, icmp_hdr->seq, time);
		return (0);
}

char *get_ip_by_hostname(char *hostname)
{
  struct hostent	*he;
  struct in_addr	**addr_list;

	he = gethostbyname(hostname);
	addr_list = (struct in_addr **)he->h_addr_list;
	return (inet_ntoa(*addr_list[0]));
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

int main(int argc, char **argv)
{
	int					sock;
	struct sockaddr_in	dst;
	int					seq = 1;
	double				start;

	if (argc != 2 || argv[1] == NULL || argv[1][0] == 0)
	{
		fprintf(stderr, "ERROR : usage : ping {-v?} [ADRESS]\n");
		return (0);
	}
	if (inet_aton(argv[1], (struct in_addr *)&dst.sin_addr.s_addr) == 0 && gethostbyname(argv[1]) == NULL)
	{
		fprintf(stderr, "ERROR : %s is an invalid adress\n", argv[1]);
		return (0);
	}
	dst.sin_family = AF_INET;
	dst.sin_port = 0;
	sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	ip = get_ip_by_hostname(argv[1]);
	if (sock < 0)
	{
		fprintf(stderr, "ERROR : socket() failed\n");
		return (0);
	}
	while (loop)
	{
		start = get_timestamp();
		ft_ping(sock, seq, dst);
		ft_recv(sock, seq, ip, start);
		seq++;
		sleep(1);
	}
	printf("--- %s ping statistics ---\n", ip);
	printf("%d packed transmitted %d received, %5.1f%% packet loss", n_packet_sent, n_packet_recv, (double)(n_packet_sent / n_packet_sent) * 100);
	close(sock);
	return (0);
}
