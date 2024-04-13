void _cg_memcpy(void *dest, const void *src, unsigned int n, const char *file, const char *func, const int line)
{
	if (unlikely(n < 1 || n > (1ul << 31))) {
		applog(LOG_ERR, "ERR: Asked to memcpy %u bytes from %s %s():%d",
			      n, file, func, line);
		return;
	}
	memcpy(dest, src, n);
}
