void cgsleep_us_r(cgtimer_t *ts_start, int64_t us)
{
	struct timespec ts_diff;

	us_to_timespec(&ts_diff, us);
	cgsleep_spec(&ts_diff, ts_start);
}
