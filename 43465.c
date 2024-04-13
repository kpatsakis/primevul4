void _cgsem_post(cgsem_t *cgsem, const char *file, const char *func, const int line)
{
	if (unlikely(sem_post(cgsem)))
		quitfrom(1, file, func, line, "Failed to sem_post errno=%d cgsem=0x%p", errno, cgsem);
}
