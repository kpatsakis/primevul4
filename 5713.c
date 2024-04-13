static int write_daemon_state(int fd, int state)
{
	char buffer[1024];
	int rv, size;

	size = sizeof(buffer) - 1;
	rv = snprintf(buffer, size,
			"booth_pid=%d "
			"booth_state=%s "
			"booth_type=%s "
			"booth_cfg_name='%s' "
			"booth_id=%d "
			"booth_addr_string='%s' "
			"booth_port=%d\n",
		getpid(), 
		( state == BOOTHD_STARTED  ? "started"  : 
		  state == BOOTHD_STARTING ? "starting" : 
		  "invalid"), 
		type_to_string(local->type),
		booth_conf->name,
		local->site_id,
		local->addr_string,
		booth_conf->port);

	if (rv < 0 || rv == size) {
		log_error("Buffer filled up in write_daemon_state().");
		return -1;
	}
	size = rv;


	rv = ftruncate(fd, 0);
	if (rv < 0) {
		log_error("lockfile %s truncate error %d: %s",
				cl.lockfile, errno, strerror(errno));
		return rv;
	}


	rv = lseek(fd, 0, SEEK_SET);
	if (rv < 0) {
		log_error("lseek set fd(%d) offset to 0 error, return(%d), message(%s)",
			fd, rv, strerror(errno));
		rv = -1;
		return rv;
	} 


	rv = write(fd, buffer, size);

	if (rv != size) {
		log_error("write to fd(%d, %d) returned %d, errno %d, message(%s)",
                      fd, size,
		      rv, errno, strerror(errno));
		return -1;
	}

	return 0;
}