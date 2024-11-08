#include "ft_ping.h"

uint16_t make_checksum(uint16_t *data, int len)
{
	// make the checksum of data
	uint32_t checksum = 0;
	int i = 0;

	while (len > 1)
	{
		checksum += data[i++];
		len -= 2;
	}
	if (len == 1)
		checksum += ((uint8_t *)data)[i];

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

struct in_addr get_addr_by_hostname(char *hostname)
{
	struct hostent	*he;
	struct in_addr	**addr_list;

	he = gethostbyname(hostname);
	addr_list = (struct in_addr **)he->h_addr_list;
	return (*addr_list[0]);
}

void fill_timestamp_array(struct packet_stats *stats, double time)
{
    stats->timestamp_array[stats->n_packet_recv] = time;
}

double get_min(double *timestamp_array)
{
	// get the smallest element of the timestamp_array
    double min = timestamp_array[0];
    for (int i = 1; timestamp_array[i]; i++)
    {
        if (min > timestamp_array[i])
            min = timestamp_array[i];
    }
    return (min);
}

double get_max(double *timestamp_array)
{
	// get the biggest element of the timestamp_array
    double max = timestamp_array[0];
    for (int i = 1; timestamp_array[i]; i++)
    {
        if (max < timestamp_array[i])
            max = timestamp_array[i];
    }
    return (max);
}

double get_avg(double *timestamp_array)
{
	// get the average of elements in timestamp_array
    double avg = 0;
    int i = 0;

    while (timestamp_array[i])
    {
        avg += timestamp_array[i];
        i++;
    }
    return (avg /= i);
}

double get_stddev(double *timestamp_array)
{
	// get the standard deviation of elements in timestamp_array
    float avg = get_avg(timestamp_array);
    float variance = 0;
    float variance_tmp;

    for (int i = 0; timestamp_array[i]; i++)
    {
        variance_tmp = timestamp_array[i] - avg;
        variance += variance_tmp * variance_tmp;
    }
    return (sqrt(variance));
}
