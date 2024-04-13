double us_tdiff(struct timeval *end, struct timeval *start)
{
	/* Sanity check. We should only be using this for small differences so
	 * limit the max to 60 seconds. */
	if (unlikely(end->tv_sec - start->tv_sec > 60))
		return 60000000;
	return (end->tv_sec - start->tv_sec) * 1000000 + (end->tv_usec - start->tv_usec);
}
