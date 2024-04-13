monitor_read(struct monitor *pmonitor, struct mon_table *ent,
    struct mon_table **pent)
{
	Buffer m;
	int ret;
	u_char type;
	struct pollfd pfd[2];

	for (;;) {
		memset(&pfd, 0, sizeof(pfd));
		pfd[0].fd = pmonitor->m_sendfd;
		pfd[0].events = POLLIN;
		pfd[1].fd = pmonitor->m_log_recvfd;
		pfd[1].events = pfd[1].fd == -1 ? 0 : POLLIN;
		if (poll(pfd, pfd[1].fd == -1 ? 1 : 2, -1) == -1) {
			if (errno == EINTR || errno == EAGAIN)
				continue;
			fatal("%s: poll: %s", __func__, strerror(errno));
		}
		if (pfd[1].revents) {
			/*
			 * Drain all log messages before processing next
			 * monitor request.
			 */
			monitor_read_log(pmonitor);
			continue;
		}
		if (pfd[0].revents)
			break;  /* Continues below */
	}

	buffer_init(&m);

	mm_request_receive(pmonitor->m_sendfd, &m);
	type = buffer_get_char(&m);

	debug3("%s: checking request %d", __func__, type);

	while (ent->f != NULL) {
		if (ent->type == type)
			break;
		ent++;
	}

	if (ent->f != NULL) {
		if (!(ent->flags & MON_PERMIT))
			fatal("%s: unpermitted request %d", __func__,
			    type);
		ret = (*ent->f)(pmonitor->m_sendfd, &m);
		buffer_free(&m);

		/* The child may use this request only once, disable it */
		if (ent->flags & MON_ONCE) {
			debug2("%s: %d used once, disabling now", __func__,
			    type);
			ent->flags &= ~MON_PERMIT;
		}

		if (pent != NULL)
			*pent = ent;

		return ret;
	}

	fatal("%s: unsupported request: %d", __func__, type);

	/* NOTREACHED */
	return (-1);
}
