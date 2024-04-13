static int loop(int fd)
{
	void (*workfn) (int ci);
	void (*deadfn) (int ci);
	int rv, i;

	rv = setup_transport();
	if (rv < 0)
		goto fail;

	rv = setup_ticket();
	if (rv < 0)
		goto fail;

	rv = write_daemon_state(fd, BOOTHD_STARTED);
	if (rv != 0) {
		log_error("write daemon state %d to lockfile error %s: %s",
                      BOOTHD_STARTED, cl.lockfile, strerror(errno));
		goto fail;
	}

	log_info("BOOTH %s daemon started, node id is 0x%08X (%d).",
		type_to_string(local->type),
			local->site_id, local->site_id);

	while (1) {
		rv = poll(pollfds, client_maxi + 1, poll_timeout);
		if (rv == -1 && errno == EINTR)
			continue;
		if (rv < 0) {
			log_error("poll failed: %s (%d)", strerror(errno), errno);
			goto fail;
		}

		for (i = 0; i <= client_maxi; i++) {
			if (clients[i].fd < 0)
				continue;

			if (pollfds[i].revents & POLLIN) {
				workfn = clients[i].workfn;
				if (workfn)
					workfn(i);
			}
			if (pollfds[i].revents &
					(POLLERR | POLLHUP | POLLNVAL)) {
				deadfn = clients[i].deadfn;
				if (deadfn)
					deadfn(i);
			}
		}

		process_tickets();

		if (process_signals() != 0) {
			return 0;
		}
	}

	return 0;

fail:
	return -1;
}