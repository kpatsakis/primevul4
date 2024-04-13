void cgsleep_ms(int ms)
{
	cgtimer_t ts_start;

	cgsleep_prepare_r(&ts_start);
	cgsleep_ms_r(&ts_start, ms);
}
