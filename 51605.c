isodate17(const unsigned char *v)
{
	struct tm tm;
	int offset;
	time_t t;

	memset(&tm, 0, sizeof(tm));
	tm.tm_year = (v[0] - '0') * 1000 + (v[1] - '0') * 100
	    + (v[2] - '0') * 10 + (v[3] - '0')
	    - 1900;
	tm.tm_mon = (v[4] - '0') * 10 + (v[5] - '0');
	tm.tm_mday = (v[6] - '0') * 10 + (v[7] - '0');
	tm.tm_hour = (v[8] - '0') * 10 + (v[9] - '0');
	tm.tm_min = (v[10] - '0') * 10 + (v[11] - '0');
	tm.tm_sec = (v[12] - '0') * 10 + (v[13] - '0');
	/* v[16] is the signed timezone offset, in 1/4-hour increments. */
	offset = ((const signed char *)v)[16];
	if (offset > -48 && offset < 52) {
		tm.tm_hour -= offset / 4;
		tm.tm_min -= (offset % 4) * 15;
	}
	t = time_from_tm(&tm);
	if (t == (time_t)-1)
		return ((time_t)0);
	return (t);
}
