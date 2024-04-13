int update_authkey()
{
	struct stat statbuf;

	if (stat(booth_conf->authfile, &statbuf) < 0) {
		log_error("cannot stat authentication file %s: %s",
			booth_conf->authfile, strerror(errno));
		return -1;
	}
	if (statbuf.st_mtime > booth_conf->authstat.st_mtime) {
		return read_authkey();
	}
	return 0;
}