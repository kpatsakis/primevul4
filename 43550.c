void us_to_timeval(struct timeval *val, int64_t us)
{
	lldiv_t tvdiv = lldiv(us, 1000000);

	val->tv_sec = tvdiv.quot;
	val->tv_usec = tvdiv.rem;
}
