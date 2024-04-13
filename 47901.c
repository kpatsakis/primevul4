static void pid_from_ns(int sock, pid_t tpid)
{
	pid_t vpid;
	struct ucred cred;
	char v;
	struct timeval tv;
	fd_set s;
	int ret;

	cred.uid = 0;
	cred.gid = 0;
	while (1) {
		FD_ZERO(&s);
		FD_SET(sock, &s);
		tv.tv_sec = 2;
		tv.tv_usec = 0;
		ret = select(sock+1, &s, NULL, NULL, &tv);
		if (ret <= 0) {
			fprintf(stderr, "%s: bad select before read from parent: %s\n",
				__func__, strerror(errno));
			_exit(1);
		}
		if ((ret = read(sock, &vpid, sizeof(pid_t))) != sizeof(pid_t)) {
			fprintf(stderr, "%s: bad read from parent: %s\n",
				__func__, strerror(errno));
			_exit(1);
		}
		if (vpid == -1) // done
			break;
		v = '0';
		cred.pid = vpid;
		if (send_creds(sock, &cred, v, true) != SEND_CREDS_OK) {
			v = '1';
			cred.pid = getpid();
			if (send_creds(sock, &cred, v, false) != SEND_CREDS_OK)
				_exit(1);
		}
	}
	_exit(0);
}
