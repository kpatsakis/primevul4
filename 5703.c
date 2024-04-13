static int create_lockfile(void)
{
	int rv, fd;

	fd = -1;
	rv = _lockfile(O_CREAT | O_WRONLY, &fd, NULL);

	if (fd == -1) {
		log_error("lockfile %s open error %d: %s",
				cl.lockfile, rv, strerror(rv));
		return -1;
	}

	if (rv < 0) {
		log_error("lockfile %s setlk error %d: %s",
				cl.lockfile, rv, strerror(rv));
		goto fail;
	}

	rv = write_daemon_state(fd, BOOTHD_STARTING);
	if (rv != 0) {
		log_error("write daemon state %d to lockfile error %s: %s",
				BOOTHD_STARTING, cl.lockfile, strerror(errno));
		goto fail;
	}

	if (is_root()) {
		if (fchown(fd, booth_conf->uid, booth_conf->gid) < 0)
			log_error("fchown() on lockfile said %d: %s",
					errno, strerror(errno));
	}

	return fd;

fail:
	close(fd);
	return -1;
}