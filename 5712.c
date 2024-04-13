static int do_status(int type)
{
	pid_t pid;
	int rv, status_lock_fd, ret;
	const char *reason = NULL;
	char lockfile_data[1024], *cp;


	ret = PCMK_OCF_NOT_RUNNING;

	rv = setup_config(type);
	if (rv) {
		reason = "Error reading configuration.";
		ret = PCMK_OCF_UNKNOWN_ERROR;
		goto quit;
	}


	if (!local) {
		reason = "No Service IP active here.";
		goto quit;
	}


	rv = _lockfile(O_RDWR, &status_lock_fd, &pid);
	if (status_lock_fd == -1) {
		reason = "No PID file.";
		goto quit;
	}
	if (rv == 0) {
		close(status_lock_fd);
		reason = "PID file not locked.";
		goto quit;
	}
	if (pid) {
		fprintf(stdout, "booth_lockpid=%d ", pid);
		fflush(stdout);
	}

	rv = read(status_lock_fd, lockfile_data, sizeof(lockfile_data) - 1);
	if (rv < 4) {
		close(status_lock_fd);
		reason = "Cannot read lockfile data.";
		ret = PCMK_LSB_UNKNOWN_ERROR;
		goto quit;
	}
	lockfile_data[rv] = 0;

	close(status_lock_fd);


	/* Make sure it's only a single line */
	cp = strchr(lockfile_data, '\r');
	if (cp)
		*cp = 0;
	cp = strchr(lockfile_data, '\n');
	if (cp)
		*cp = 0;



	rv = setup_tcp_listener(1);
	if (rv == 0) {
		reason = "TCP port not in use.";
		goto quit;
	}


	fprintf(stdout, "booth_lockfile='%s' %s\n",
			cl.lockfile, lockfile_data);
	if (!daemonize)
		fprintf(stderr, "Booth at %s port %d seems to be running.\n",
				local->addr_string, booth_conf->port);
	return 0;


quit:
	log_debug("not running: %s", reason);
	/* Ie. "DEBUG" */
	if (!daemonize)
		fprintf(stderr, "not running: %s\n", reason);
	return ret;
}