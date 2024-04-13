bool time_less(struct timeval *a, struct timeval *b)
{
	return timercmp(a, b, <);
}
