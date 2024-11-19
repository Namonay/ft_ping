#include "ft_ping.h"
#include "icmp_codes.h"

static const char *get_error_message(const uint8_t type, const uint8_t code)
{
    switch(type)
    {
        case ICMP_DEST_UNREACH:
            return (net_icmp_unreach_messages[code]);
        case ICMP_REDIRECT:
            return (net_icmp_redirect_messages[code]);
        case ICMP_TIME_EXCEEDED:
            return (net_icmp_time_messages[code]);
    }
    return (NULL);
}
bool parse_packet(const struct net_packet packet, const uint16_t seq, const uint16_t checksum)
{
    if (packet.icmp_hdr->seq != seq || make_checksum((uint16_t *)packet.icmp_hdr, sizeof(*packet.icmp_hdr)) != checksum)
		return (false);
    if (packet.icmp_hdr->type != ICMP_ECHOREPLY)
        return (false);
    return (true);
}

void print_packet_error(const struct net_packet packet)
{
    fprintf(stderr, "%d bytes from %s:  %s\n", packet.n_bytes, inet_ntoa(packet.addr.sin_addr), get_error_message(packet.icmp_hdr->type, packet.icmp_hdr->code));
}