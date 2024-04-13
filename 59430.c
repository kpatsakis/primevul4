lha_win_time(uint64_t wintime, long *ns)
{
#define EPOC_TIME ARCHIVE_LITERAL_ULL(116444736000000000)

	if (wintime >= EPOC_TIME) {
		wintime -= EPOC_TIME;	/* 1970-01-01 00:00:00 (UTC) */
		if (ns != NULL)
			*ns = (long)(wintime % 10000000) * 100;
		return (wintime / 10000000);
	} else {
		if (ns != NULL)
			*ns = 0;
		return (0);
	}
}
