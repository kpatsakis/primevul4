static int read_authkey()
{
	int fd;

	booth_conf->authkey[0] = '\0';
	fd = open(booth_conf->authfile, O_RDONLY);
	if (fd < 0) {
		log_error("cannot open %s: %s",
			booth_conf->authfile, strerror(errno));
		return -1;
	}
	if (fstat(fd, &booth_conf->authstat) < 0) {
		log_error("cannot stat authentication file %s (%d): %s",
			booth_conf->authfile, fd, strerror(errno));
		close(fd);
		return -1;
	}
	if (booth_conf->authstat.st_mode & (S_IRGRP | S_IROTH)) {
		log_error("%s: file shall not be readable for anyone but the owner",
			booth_conf->authfile);
		close(fd);
		return -1;
	}
	booth_conf->authkey_len = read(fd, booth_conf->authkey, BOOTH_MAX_KEY_LEN);
	close(fd);
	trim_key();
	log_debug("read key of size %d in authfile %s",
		booth_conf->authkey_len, booth_conf->authfile);
	/* make sure that the key is of minimum length */
	return (booth_conf->authkey_len >= BOOTH_MIN_KEY_LEN) ? 0 : -1;
}