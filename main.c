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


int ft_ping(int sock, int *seq, struct sockaddr_in src, struct sockaddr_in dst)
{
	unsigned char data[2048];
	unsigned char data2[2048];
	int len;
	struct icmp_header *icmp_hdr = (struct icmp_header *)data;

	memset(data, 0, sizeof(data));
	memset(data2, 0, sizeof(data2));
	sock = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sock < 0)
	{
		fprintf(stderr, "ERROR : socket() failed\n");
		return (0);
	}
	if (getsockname(sock, (struct sockaddr *)&src, &len) < 0)
	{
		fprintf(stderr, "ERROR : getsockname() failed\n");
		return (0);
	}
	dst.sin_family = AF_INET;
	memset(icmp_hdr, 0, sizeof(icmp_hdr));
	icmp_hdr->type = ICMP_ECHO;
	icmp_hdr->code = 0;
	icmp_hdr->checksum = 0;
	icmp_hdr->id = getpid();
	icmp_hdr->seq = *seq++;
	icmp_hdr->checksum = calculate_checksum((uint16_t *)icmp_hdr, sizeof(icmp_hdr));
	if (sendto(sock, data, sizeof(data), 0, (struct sockaddr *)&dst, sizeof(dst)) == -1)
	{
		fprintf(stderr, "ERROR : sendto() failed\n");
		return (0);
	}
	if (recvfrom(sock, data2, sizeof(data2), 0, (struct sockaddr *)&src, &len) == -1)
	{
		fprintf(stderr, "ERROR : recvfrom() failed\n");
		return (0);
	}
	struct icmp_header *icmp_recv_hdr = (struct icmp_header *)(data2 + 20);
	for (int i = 0; data[i]; i++)
		printf("%c", data[i]);
	for (int i = 0; data2[i]; i++)
		printf("%c", data2[i]);
	if (icmp_recv_hdr->type != ICMP_ECHOREPLY || icmp_recv_hdr->code != 0)
	{
		printf("%d %d\n", icmp_recv_hdr->type, icmp_recv_hdr->code);
		fprintf(stderr, "ERROR : invalid packet received (code)\n");
		return (0);
	}
	if (icmp_recv_hdr->id != icmp_hdr->id)
	{
		fprintf(stderr, "ERROR : invalid packet received (id is not matching)\n");
		return (0);
	}
	if (icmp_recv_hdr->seq != icmp_hdr->seq)
	{
		fprintf(stderr, "ERROR : invalid packet received (sequence is not matching)\n");
		return (0);
	}
	printf("success");
	close(sock);
	return (1);
}

int main(int argc, char **argv)
{
	int sock;
	struct sockaddr_in src, dst;
	int seq = 1;
	src.sin_family = AF_INET;
	if (argc != 2 || argv[1] == NULL || argv[1][0] == 0)
	{
		fprintf(stderr, "ERROR : usage : ping {-v?} [ADRESS]\n");
		return (0);
	}
	if (gethostbyname(argv[1]) == NULL && inet_aton(argv[1], (struct in_addr *)&dst.sin_addr.s_addr) == 0)
	{
		fprintf(stderr, "ERROR : %s is an invalid adress\n", argv[1]);
		return (0);
	}
	return (ft_ping(sock, &seq, src, dst));
}