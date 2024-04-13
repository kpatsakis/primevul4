void _cgsem_init(cgsem_t *cgsem, const char *file, const char *func, const int line)
{
	int flags, fd, i;

	if (pipe(cgsem->pipefd) == -1)
		quitfrom(1, file, func, line, "Failed pipe errno=%d", errno);

	/* Make the pipes FD_CLOEXEC to allow them to close should we call
	 * execv on restart. */
	for (i = 0; i < 2; i++) {
		fd = cgsem->pipefd[i];
		flags = fcntl(fd, F_GETFD, 0);
		flags |= FD_CLOEXEC;
		if (fcntl(fd, F_SETFD, flags) == -1)
			quitfrom(1, file, func, line, "Failed to fcntl errno=%d", errno);
	}
}
