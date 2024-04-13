static pid_t get_task_reaper_pid(pid_t task)
{
	int sock[2];
	pid_t pid;
	pid_t ret = -1;
	char v = '0';
	struct ucred cred;

	if (socketpair(AF_UNIX, SOCK_DGRAM, 0, sock) < 0) {
		perror("socketpair");
		return -1;
	}

	pid = fork();
	if (pid < 0)
		goto out;
	if (!pid) {
		close(sock[1]);
		write_task_init_pid_exit(sock[0], task);
	}

	if (!recv_creds(sock[1], &cred, &v))
		goto out;
	ret = cred.pid;

out:
	close(sock[0]);
	close(sock[1]);
	return ret;
}
