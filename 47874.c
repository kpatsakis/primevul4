static long int get_pid1_time(pid_t pid)
{
	char fnam[100];
	int fd, cpipe[2], ret;
	struct stat sb;
	pid_t cpid;
	struct timeval tv;
	fd_set s;
	char v;

	if (unshare(CLONE_NEWNS))
		return 0;

	if (mount(NULL, "/", NULL, MS_SLAVE|MS_REC, NULL)) {
		perror("rslave mount failed");
		return 0;
	}

	ret = snprintf(fnam, sizeof(fnam), "/proc/%d/ns/pid", pid);
	if (ret < 0 || ret >= sizeof(fnam))
		return 0;

	fd = open(fnam, O_RDONLY);
	if (fd < 0) {
		perror("get_pid1_time open of ns/pid");
		return 0;
	}
	if (setns(fd, 0)) {
		perror("get_pid1_time setns 1");
		close(fd);
		return 0;
	}
	close(fd);

	if (pipe(cpipe) < 0)
		exit(1);

loop:
	cpid = fork();
	if (cpid < 0)
		return 0;

	if (!cpid) {
		char b = '1';
		close(cpipe[0]);
		if (write(cpipe[1], &b, sizeof(char)) < 0) {
			fprintf(stderr, "%s (child): erorr on write: %s\n",
				__func__, strerror(errno));
		}
		close(cpipe[1]);
		umount2("/proc", MNT_DETACH);
		if (mount("proc", "/proc", "proc", 0, NULL)) {
			perror("get_pid1_time mount");
			return 0;
		}
		ret = lstat("/proc/1", &sb);
		if (ret) {
			perror("get_pid1_time lstat");
			return 0;
		}
		return time(NULL) - sb.st_ctime;
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

	wait_for_pid(cpid);
	_exit(0);

again:
	kill(cpid, SIGKILL);
	wait_for_pid(cpid);
	goto loop;
}
