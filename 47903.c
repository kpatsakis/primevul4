static void pid_to_ns(int sock, pid_t tpid)
{
	char v = '0';
	struct ucred cred;

	while (recv_creds(sock, &cred, &v)) {
		if (v == '1')
			_exit(0);
		if (write(sock, &cred.pid, sizeof(pid_t)) != sizeof(pid_t))
			_exit(1);
	}
	_exit(0);
}
