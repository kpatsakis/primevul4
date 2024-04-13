print_sep(struct magic_set *ms, int firstline)
{
	if (ms->flags & MAGIC_MIME)
		return 0;
	if (firstline)
		return 0;
	/*
	 * we found another match
	 * put a newline and '-' to do some simple formatting
	 */
	return file_printf(ms, "\n- ");
}
