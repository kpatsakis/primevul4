static void test_two_locks(void)
{
	struct lxc_lock *l;
	pid_t pid;
	int ret, status;
	int p[2];
	char c;

	if (pipe(p) < 0)
		exit(1);
	if ((pid = fork()) < 0)
		exit(1);
	if (pid == 0) {
		if (read(p[0], &c, 1) < 0) {
			perror("read");
			exit(1);
		}
		l = lxc_newlock("/tmp", "lxctest-sem");
		if (!l) {
			fprintf(stderr, "%d: child: failed to create lock\n", __LINE__);
			exit(1);
		}
		if (lxclock(l, 0) < 0) {
			fprintf(stderr, "%d: child: failed to grab lock\n", __LINE__);
			exit(1);
		}
		fprintf(stderr, "%d: child: grabbed lock\n", __LINE__);
		exit(0);
	}
	l = lxc_newlock("/tmp", "lxctest-sem");
	if (!l) {
		fprintf(stderr, "%d: failed to create lock\n", __LINE__);
		exit(1);
	}
	if (lxclock(l, 0) < 0) {
		fprintf(stderr, "%d; failed to get lock\n", __LINE__);
		exit(1);
	}
	if (write(p[1], "a", 1) < 0) {
		perror("write");
		exit(1);
	}
	sleep(3);
	ret = waitpid(pid, &status, WNOHANG);
	if (ret == pid) { // task exited
		if (WIFEXITED(status)) {
			printf("%d exited normally with exit code %d\n", pid,
				WEXITSTATUS(status));
			if (WEXITSTATUS(status) == 0)
				exit(1);
		} else
			printf("%d did not exit normally\n", pid);
		return;
	} else if (ret < 0) {
		perror("waitpid");
		exit(1);
	}
	kill(pid, SIGKILL);
	wait(&status);
	close(p[1]);
	close(p[0]);
	lxcunlock(l);
	lxc_putlock(l);
}
