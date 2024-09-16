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

int ft_ping(int sock, int seq, struct sockaddr_in dst)
{
	unsigned char data[2048];
	int len;
	struct icmp_header *icmp_hdr = (struct icmp_header *)data;

	memset(data, 0, sizeof(data));
	sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sock < 0)
	{
		fprintf(stderr, "ERROR : socket() failed\n");
		return (0);
	}
	dst.sin_family = AF_INET;
	memset(icmp_hdr, 0, sizeof(icmp_hdr));
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

int ft_recv(int sock, int seq, char *ip)
{
	unsigned char data[2048];
	struct icmp_header *icmp_recv_hdr = (struct icmp_header *)(data + 20);
	int len;
	struct sockaddr_in addr;
	recvfrom(sock, data, sizeof(data), 0, &addr, &len);

	// while (recvfrom(sock, data, sizeof(data), 0, &addr, &len) > 0 && icmp_recv_hdr->type != 0);
	printf("%d bytes from %s: icmp_seq:%d time:placeholder", len, ip, icmp_recv_hdr->seq);

}

int main(int argc, char **argv)
{
	int sock;
	struct sockaddr_in dst;
	int seq = 1;
	char *ip;
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
	ip = inet_ntoa(dst.sin_addr);
	bind(sock, &dst, sizeof(dst));
	while (1)
	{
		ft_ping(sock, seq, dst);
		ft_recv(sock, seq, ip);
		seq++;
		sleep(1);
	}
	close(sock);
	return (0);
}