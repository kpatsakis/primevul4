static void pid_from_ns_wrapper(int sock, pid_t tpid)
{
	int newnsfd = -1, ret, cpipe[2];
	char fnam[100];
	pid_t cpid;
	fd_set s;
	struct timeval tv;
	char v;

	ret = snprintf(fnam, sizeof(fnam), "/proc/%d/ns/pid", tpid);
	if (ret < 0 || ret >= sizeof(fnam))
		_exit(1);
	newnsfd = open(fnam, O_RDONLY);
	if (newnsfd < 0)
		_exit(1);
	if (setns(newnsfd, 0) < 0)
		_exit(1);
	close(newnsfd);

	if (pipe(cpipe) < 0)
		_exit(1);

loop:
	cpid = fork();

	if (cpid < 0)
		_exit(1);

	if (!cpid) {
		char b = '1';
		close(cpipe[0]);
		if (write(cpipe[1], &b, sizeof(char)) < 0) {
			fprintf(stderr, "%s (child): erorr on write: %s\n",
				__func__, strerror(errno));
		}
		close(cpipe[1]);
		pid_from_ns(sock, tpid);
	}

	FD_ZERO(&s);
	FD_SET(cpipe[0], &s);
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	ret = select(cpipe[0]+1, &s, NULL, NULL, &tv);
	if (ret <= 0)
		goto again;
	ret = read(cpipe[0], &v, 1);
	if (ret != sizeof(char) || v != '1') {
		goto again;
	}

	if (!wait_for_pid(cpid))
		_exit(1);
	_exit(0);

again:
	kill(cpid, SIGKILL);
	wait_for_pid(cpid);
 	goto loop;
 }
