checkdone(struct magic_set *ms, int *rv)
{
	if ((ms->flags & MAGIC_CONTINUE) == 0)
		return 1;
	if (file_printf(ms, "\n- ") == -1)
		*rv = -1;
	return 0;
}
