get_rtpref(u_int v)
{
	static const char *rtpref_str[] = {
		"medium",		/* 00 */
		"high",			/* 01 */
		"rsv",			/* 10 */
		"low"			/* 11 */
	};

	return rtpref_str[((v & ND_RA_FLAG_RTPREF_MASK) >> 3) & 0xff];
}
