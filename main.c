#include "ft_ping.h"

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
	printf("checksum is : %X\n", (uint16_t)~checksum);
	return (~checksum);
}


int ft_ping(struct t_socket sock, int *seq)
{
	unsigned char data[2048];
	unsigned char data2[2048];
	struct icmphdr *icmp_header = (struct icmphdr *)data;

	memset(data, 0, sizeof(data));
	memset(data2, 0, sizeof(data2));
	sock.fd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sock.fd < 0)
	{
		printf(strerror(errno));
		fprintf(stderr, "ERROR : socket() failed\n");
		return (0);
	}
	if (getsockname(sock.fd, (struct sockaddr *)&sock.src, &sock.len) < 0)
	{
		fprintf(stderr, "ERROR : getsockname() failed\n");
		return (0);
	}
	sock.dst.sin_family = AF_INET;
	memset(icmp_header, 0, sizeof(icmp_header));
	icmp_header->type = ICMP_ECHO;
	icmp_header->code = 0;
	icmp_header->checksum = 0;
	icmp_header->un.echo.id = getpid();
	icmp_header->un.echo.sequence = *seq++;
	icmp_header->checksum = calculate_checksum((uint16_t *)icmp_header, sizeof(icmp_header));
	if (sendto(sock.fd, data, sizeof(data), 0, (struct sockaddr *)&sock.dst, sizeof(sock.dst)) == -1)
	{
		fprintf(stderr, "ERROR : sendto() failed\n");
		return (0);
	}
	if (recvfrom(sock.fd, data2, sizeof(data2), 0, (struct sockaddr *)&sock.src, &sock.len) == -1)
	{
		fprintf(stderr, "ERROR : recvfrom() failed\n");
		return (0);
	}
	struct icmphdr *icmp_recv_header = (struct icmphdr *)(data2 + 20);
	if (icmp_recv_header->type != ICMP_ECHOREPLY || icmp_recv_header->code != 0)
	{
		fprintf(stderr, "ERROR : invalid packet received (code)\n");
		return (0);
	}
	printf("success");
	close(sock.fd);
	return (1);
}

int main(int argc, char **argv)
{
	struct t_socket sock;
	int seq = 1;
	sock.src.sin_family = AF_INET;
	if (argc != 2 || argv[1] == NULL || argv[1][0] == 0)
	{
		fprintf(stderr, "ERROR : usage : ping {-v?} [ADRESS]\n");
		return (0);
	}
	if (gethostbyname(argv[1]) == NULL && inet_aton(argv[1], (struct in_addr *)&sock.dst.sin_addr.s_addr) == 0)
	{
		fprintf(stderr, "ERROR : %s is an invalid adress\n", argv[1]);
		return (0);
	}
	return (ft_ping(sock, &seq));
}