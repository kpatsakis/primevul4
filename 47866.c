static bool do_read_pids(pid_t tpid, const char *contrl, const char *cg, const char *file, char **d)
{
	int sock[2] = {-1, -1};
	char *tmpdata = NULL;
	int ret;
	pid_t qpid, cpid = -1;
	bool answer = false;
	char v = '0';
	struct ucred cred;
	struct timeval tv;
	size_t sz = 0, asz = 0;
	fd_set s;

	if (!cgfs_get_value(contrl, cg, file, &tmpdata))
		return false;

	/*
	 * Now we read the pids from returned data one by one, pass
	 * them into a child in the target namespace, read back the
	 * translated pids, and put them into our to-return data
	 */

	if (socketpair(AF_UNIX, SOCK_DGRAM, 0, sock) < 0) {
		perror("socketpair");
		free(tmpdata);
		return false;
	}

	cpid = fork();
	if (cpid == -1)
		goto out;

	if (!cpid) // child
		pid_to_ns_wrapper(sock[1], tpid);

	char *ptr = tmpdata;
	cred.uid = 0;
	cred.gid = 0;
	while (sscanf(ptr, "%d\n", &qpid) == 1) {
		cred.pid = qpid;
		ret = send_creds(sock[0], &cred, v, true);

		if (ret == SEND_CREDS_NOTSK)
			goto next;
		if (ret == SEND_CREDS_FAIL)
			goto out;

		FD_ZERO(&s);
		FD_SET(sock[0], &s);
		tv.tv_sec = 2;
		tv.tv_usec = 0;
		ret = select(sock[0]+1, &s, NULL, NULL, &tv);
		if (ret <= 0) {
			fprintf(stderr, "%s: select error waiting for pid from child: %s\n",
				__func__, strerror(errno));
			goto out;
		}
		if (read(sock[0], &qpid, sizeof(qpid)) != sizeof(qpid)) {
			fprintf(stderr, "%s: error reading pid from child: %s\n",
				__func__, strerror(errno));
			goto out;
		}
		must_strcat_pid(d, &sz, &asz, qpid);
next:
		ptr = strchr(ptr, '\n');
		if (!ptr)
			break;
		ptr++;
	}

	cred.pid = getpid();
	v = '1';
	if (send_creds(sock[0], &cred, v, true) != SEND_CREDS_OK) {
		fprintf(stderr, "%s: failed to ask child to exit: %s\n",
			__func__, strerror(errno));
		goto out;
	}

	answer = true;

out:
	free(tmpdata);
	if (cpid != -1)
		wait_for_pid(cpid);
	if (sock[0] != -1) {
		close(sock[0]);
		close(sock[1]);
	}
	return answer;
}
