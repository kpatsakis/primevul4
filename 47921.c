void write_task_init_pid_exit(int sock, pid_t target)
{
	struct ucred cred;
	char fnam[100];
	pid_t pid;
	char v;
	int fd, ret;

	ret = snprintf(fnam, sizeof(fnam), "/proc/%d/ns/pid", (int)target);
	if (ret < 0 || ret >= sizeof(fnam))
		exit(1);

	fd = open(fnam, O_RDONLY);
	if (fd < 0) {
		perror("get_pid1_time open of ns/pid");
		exit(1);
	}
	if (setns(fd, 0)) {
		perror("get_pid1_time setns 1");
		close(fd);
		exit(1);
	}
	pid = fork();
	if (pid < 0)
		exit(1);
	if (pid != 0) {
		wait_for_pid(pid);
		exit(0);
	}

	/* we are the child */
	cred.uid = 0;
	cred.gid = 0;
	cred.pid = 1;
	v = '1';
	send_creds(sock, &cred, v, true);
	exit(0);
}
