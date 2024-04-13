void _cgsem_wait(cgsem_t *cgsem, const char *file, const char *func, const int line)
{
	char buf;
	int ret;
retry:
	ret = read(cgsem->pipefd[0], &buf, 1);
	if (unlikely(ret == 0))
		applog(LOG_WARNING, "Failed to read errno=%d" IN_FMT_FFL, errno, file, func, line);
	else if (unlikely(ret < 0 && interrupted))
		goto retry;
}
