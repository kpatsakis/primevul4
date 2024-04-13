monitor_openfds(struct monitor *mon, int do_logfds)
{
	int pair[2];

	if (socketpair(AF_UNIX, SOCK_STREAM, 0, pair) == -1)
		fatal("%s: socketpair: %s", __func__, strerror(errno));
	FD_CLOSEONEXEC(pair[0]);
	FD_CLOSEONEXEC(pair[1]);
	mon->m_recvfd = pair[0];
	mon->m_sendfd = pair[1];

	if (do_logfds) {
		if (pipe(pair) == -1)
			fatal("%s: pipe: %s", __func__, strerror(errno));
		FD_CLOSEONEXEC(pair[0]);
		FD_CLOSEONEXEC(pair[1]);
		mon->m_log_recvfd = pair[0];
		mon->m_log_sendfd = pair[1];
	} else
		mon->m_log_recvfd = mon->m_log_sendfd = -1;
}
