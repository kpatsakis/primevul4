file_softmagic(struct magic_set *ms, const unsigned char *buf, size_t nbytes,
    size_t level, int mode, int text)
{
	struct mlist *ml;
	int rv, printed_something = 0, need_separator = 0;

	for (ml = ms->mlist[0]->next; ml != ms->mlist[0]; ml = ml->next)
		if ((rv = match(ms, ml->magic, ml->nmagic, buf, nbytes, 0, mode,
		    text, 0, level, &printed_something, &need_separator,
		    NULL)) != 0)
			return rv;

	return 0;
}
