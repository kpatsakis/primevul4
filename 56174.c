unsigned_relts_print(netdissect_options *ndo,
                     uint32_t secs)
{
	static const char *lengths[] = {"y", "w", "d", "h", "m", "s"};
	static const u_int seconds[] = {31536000, 604800, 86400, 3600, 60, 1};
	const char **l = lengths;
	const u_int *s = seconds;

	if (secs == 0) {
		ND_PRINT((ndo, "0s"));
		return;
	}
	while (secs > 0) {
		if (secs >= *s) {
			ND_PRINT((ndo, "%d%s", secs / *s, *l));
			secs -= (secs / *s) * *s;
		}
		s++;
		l++;
	}
}
