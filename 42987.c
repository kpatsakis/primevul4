file_printf(struct magic_set *ms, const char *fmt, ...)
{
	int rv;
	va_list ap;

	va_start(ap, fmt);
	rv = file_vprintf(ms, fmt, ap);
	va_end(ap);
	return rv;
}
