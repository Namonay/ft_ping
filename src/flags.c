#include "ft_ping.h"

const char *flag_list[] = 
{
    "usage : ping [OPTIONS ...] [ADDRESS]",
	"Send ICMP ECHO_REQUEST packets to network hosts.",
	"  -c [COUNT]	stop after COUNT packets have been sent",
	"  -f			flood mode",
	"  -i [NUMBER]	send a ping at NUMBER seconds intervals",
	"  -l [NUMBER]	send NUMBER ping as fast as possible then continue in normal mode",
	"  -q			quiet mode",
	"  -t [NUMBER]  set the packet TTL (time to live) to NUMBER",
    "  -v,			verbose output",
    "  -?,			print this help page",
	NULL,
};

static void print_help(void)
{
	for (int i = 0; flag_list[i]; i++)
		printf("%s\n", flag_list[i]);
}

bool parse_opt(int argc, char **argv, struct flags *flags)
{
	int flag;
	static bool interval = false;
	while ((flag = getopt(argc, argv, "c:fi:l:qt:v?")) != -1) 
	{
		switch (flag)
		{
			case 'v':
				flags->verbose = true;
				break;

			case 'q':
				flags->quiet = true;
				break;

			case '?':
				print_help();
				return (false);

			case 'i':
				if (flags->flood)
				{
					fprintf(stderr, "%s: -f and -i flags are incompatible\n%s\n", argv[0], flag_list[0]);
					return (false);
				}
				if (atof(optarg) <= 0)
				{
					fprintf(stderr, "%s: invalid value '%s'\n%s\n", argv[0], optarg, flag_list[0]);
					return (false);
				}
				flags->interval = atof(optarg);
				interval = true;	
				break;

			case 'c':
				if (ft_atoi(optarg) == -1)
				{
					fprintf(stderr, "%s: invalid value '%s'\n%s\n", argv[0], optarg, flag_list[0]);
					return (false);
				}
				flags->count = ft_atoi(optarg);
				break;

			case 'f':
				if (interval)
				{
					fprintf(stderr, "%s: -f and -i flags are incompatible\n%s\n", argv[0], flag_list[0]);
					return (false);
				}
				flags->flood = true;
				flags->quiet = true;
				flags->interval = 0;
				break;

			case 'l':
				if (ft_atoi(optarg) == -1)
				{
					fprintf(stderr, "%s: invalid value '%s'\n%s\n", argv[0], optarg, flag_list[0]);
					return (false);
				}
				flags->preload_count = ft_atoi(optarg);
				break;
			case 't':
				if (ft_atoi(optarg) == -1)
				{
					fprintf(stderr, "%s: invalid value '%s'\n%s\n", argv[0], optarg, flag_list[0]);
					return (false);
				}
				flags->ttl = ft_atoi(optarg);
				break;
		}
	}
	if (argc < 2 || argv[optind] == NULL || argv[optind][0] == 0) // optind is a value given by getopt() that's equal to the first argument that isn't a flag
	{
		fprintf(stderr, "ERROR : usage : %s [OPTIONS ...] [ADDRESS]\n", argv[0]);
		return (false);
	}
	return (true);
}