file_regerror(file_regex_t *rx, int rc, struct magic_set *ms)
{
	char errmsg[512];

	(void)regerror(rc, &rx->rx, errmsg, sizeof(errmsg));
 	file_magerror(ms, "regex error %d for `%s', (%s)", rc, rx->pat,
 	    errmsg);
 }
