void _cgsem_init(cgsem_t *cgsem, const char *file, const char *func, const int line)
{
	int ret;
	if ((ret = sem_init(cgsem, 0, 0)))
		quitfrom(1, file, func, line, "Failed to sem_init ret=%d errno=%d", ret, errno);
}
