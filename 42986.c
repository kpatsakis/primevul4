file_printedlen(const struct magic_set *ms)
{
	return ms->o.buf == NULL ? 0 : strlen(ms->o.buf);
}
