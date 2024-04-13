void cgsleep_us(int64_t us)
{
	cgtimer_t ts_start;

	cgsleep_prepare_r(&ts_start);
	cgsleep_us_r(&ts_start, us);
}
