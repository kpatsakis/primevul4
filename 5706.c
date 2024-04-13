static int limit_this_process(void)
{
	int rv;
	if (!is_root())
		return 0;

	if (setregid(booth_conf->gid, booth_conf->gid) < 0) {
		rv = errno;
		log_error("setregid() didn't work: %s", strerror(rv));
		return rv;
	}

	if (setreuid(booth_conf->uid, booth_conf->uid) < 0) {
		rv = errno;
		log_error("setreuid() didn't work: %s", strerror(rv));
		return rv;
	}

	return 0;
}