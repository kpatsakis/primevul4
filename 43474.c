void cgsem_reset(cgsem_t *cgsem)
{
	int ret, fd;
	fd_set rd;
	char buf;

	fd = cgsem->pipefd[0];
	FD_ZERO(&rd);
	FD_SET(fd, &rd);
	do {
		struct timeval timeout = {0, 0};

		ret = select(fd + 1, &rd, NULL, NULL, &timeout);
		if (ret > 0)
			ret = read(fd, &buf, 1);
		else if (unlikely(ret < 0 && interrupted()))
			ret = 1;
	} while (ret > 0);
}
