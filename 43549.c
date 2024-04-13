void us_to_timespec(struct timespec *spec, int64_t us)
{
	lldiv_t tvdiv = lldiv(us, 1000000);

	spec->tv_sec = tvdiv.quot;
	spec->tv_nsec = tvdiv.rem * 1000;
}
