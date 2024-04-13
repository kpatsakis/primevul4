static long int getreaperage(pid_t qpid)
{
	int pid, mypipe[2], ret;
	struct timeval tv;
	fd_set s;
	long int mtime, answer = 0;

	if (pipe(mypipe)) {
		return 0;
	}

	pid = fork();

	if (!pid) { // child
		mtime = get_pid1_time(qpid);
		if (write(mypipe[1], &mtime, sizeof(mtime)) != sizeof(mtime))
			fprintf(stderr, "Warning: bad write from getreaperage\n");
		_exit(0);
	}

	close(mypipe[1]);
	FD_ZERO(&s);
	FD_SET(mypipe[0], &s);
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	ret = select(mypipe[0]+1, &s, NULL, NULL, &tv);
	if (ret <= 0) {
		perror("select");
		goto out;
	}
	if (!ret) {
		fprintf(stderr, "timed out\n");
		goto out;
	}
	if (read(mypipe[0], &mtime, sizeof(mtime)) != sizeof(mtime)) {
		perror("read");
		goto out;
	}
	answer = mtime;

out:
	wait_for_pid(pid);
	close(mypipe[0]);
	return answer;
}
