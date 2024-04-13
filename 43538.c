void timeraddspec(struct timespec *a, const struct timespec *b)
{
	a->tv_sec += b->tv_sec;
	a->tv_nsec += b->tv_nsec;
	if (a->tv_nsec >= 1000000000) {
		a->tv_nsec -= 1000000000;
		a->tv_sec++;
	}
}
