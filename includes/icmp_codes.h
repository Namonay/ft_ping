#pragma once

typedef enum
{
    ICMP_ECHOREPLY = 0,        /* Echo Reply                        */
    ICMP_DEST_UNREACH = 3,        /* Destination Unreachable        */
    ICMP_SOURCE_QUENCH = 4,        /* Source Quench                */
    ICMP_REDIRECT = 5,        /* Redirect (change route)        */
    ICMP_ECHO = 8,        /* Echo Request                        */
    ICMP_TIME_EXCEEDED = 11,        /* Time Exceeded                */
    ICMP_PARAMETERPROB = 12,        /* Parameter Problem                */
    ICMP_TIMESTAMP = 13,        /* Timestamp Request                */
    ICMP_TIMESTAMPREPLY = 14,        /* Timestamp Reply                */
    ICMP_INFO_REQUEST = 15,        /* Information Request                */
    ICMP_INFO_REPLY = 16,        /* Information Reply                */
    ICMP_ADDRESS = 17,        /* Address Mask Request                */
    ICMP_ADDRESSREPLY = 18       /* Address Mask Reply                */
}   net_icmp_types;


typedef enum
{
    ICMP_NET_UNREACH = 0,        /* Network Unreachable                */
    ICMP_HOST_UNREACH = 1,        /* Host Unreachable                */
    ICMP_PROT_UNREACH = 2,        /* Protocol Unreachable                */
    ICMP_PORT_UNREACH = 3,        /* Port Unreachable                */
    ICMP_FRAG_NEEDED = 4,        /* Fragmentation Needed/DF set        */
    ICMP_SR_FAILED = 5,        /* Source Route failed                */
    ICMP_NET_UNKNOWN = 6,
    ICMP_HOST_UNKNOWN = 7,
    ICMP_HOST_ISOLATED = 8,
    ICMP_NET_ANO = 9,
    ICMP_HOST_ANO = 10,
    ICMP_NET_UNR_TOS = 11,
    ICMP_HOST_UNR_TOS = 12,
    ICMP_PKT_FILTERED = 13,        /* Packet filtered */
    ICMP_PREC_VIOLATION = 14,        /* Precedence violation */
    ICMP_PREC_CUTOFF = 15        /* Precedence cut off */
}   net_icmp_unreach_codes; /* Codes for UNREACH*/

typedef enum
{
    ICMP_REDIR_NET = 0,
    ICMP_REDIR_HOST = 1,
    ICMP_REDIR_NETTOS = 2,
    ICMP_REDIR_HOSTTOS = 3
}   net_icmp_redirect_code; /* Codes for REDIRECT*/

typedef enum
{
    ICMP_EXC_TTL = 0,
    ICMP_EXC_FRAGTIME = 1
}   net_icmp_time_code; /* Codes for TIME_EXCEEDED. */

static const char *net_icmp_unreach_messages[] = 
{
    [ICMP_NET_UNREACH]    = "Network Unreachable",
    [ICMP_HOST_UNREACH]   = "Host Unreachable",
    [ICMP_PROT_UNREACH]   = "Protocol Unreachable",
    [ICMP_PORT_UNREACH]   = "Port Unreachable",
    [ICMP_FRAG_NEEDED]    = "Fragmentation Needed/DF set",
    [ICMP_SR_FAILED]      = "Source Route failed",
    [ICMP_NET_UNKNOWN]    = "Network Unknown",
    [ICMP_HOST_UNKNOWN]   = "Host Unknown",
    [ICMP_HOST_ISOLATED]  = "Host Isolated",
    [ICMP_NET_ANO]        = "Network Administratively Prohibited",
    [ICMP_HOST_ANO]       = "Host Administratively Prohibited",
    [ICMP_NET_UNR_TOS]    = "Network Unreachable for TOS",
    [ICMP_HOST_UNR_TOS]   = "Host Unreachable for TOS",
    [ICMP_PKT_FILTERED]   = "Packet filtered",
    [ICMP_PREC_VIOLATION] = "Precedence violation",
    [ICMP_PREC_CUTOFF]    = "Precedence cut off"
};

static const char *net_icmp_redirect_messages[] = 
{
    [ICMP_REDIR_NET]    = "Redirect for Network",
    [ICMP_REDIR_HOST]   = "Redirect for Host",
    [ICMP_REDIR_NETTOS] = "Redirect for Network with TOS",
    [ICMP_REDIR_HOSTTOS] = "Redirect for Host with TOS"
};

static const char *net_icmp_time_messages[] = 
{
    [ICMP_EXC_TTL]       = "Time-to-live exceeded",
    [ICMP_EXC_FRAGTIME]  = "Fragment Reassembly Timeout"
};