void cgsleep_us_r(cgtimer_t *ts_start, int64_t us)
{
	LARGE_INTEGER li;
	int ms;

	li.QuadPart = ts_start->QuadPart + us * 10LL;
	ms = us / 1000;
	if (!ms)
		ms = 1;
	liSleep(&li, ms);
}
