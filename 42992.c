file_vprintf(struct magic_set *ms, const char *fmt, va_list ap)
{
	int len;
	char *buf, *newstr;

	if (ms->event_flags & EVENT_HAD_ERR)
		return 0;
	len = vasprintf(&buf, fmt, ap);
	if (len < 0)
		goto out;

	if (ms->o.buf != NULL) {
		len = asprintf(&newstr, "%s%s", ms->o.buf, buf);
		free(buf);
		if (len < 0)
			goto out;
		free(ms->o.buf);
		buf = newstr;
	}
	ms->o.buf = buf;
	return 0;
out:
	file_error(ms, errno, "vasprintf failed");
	return -1;
}
