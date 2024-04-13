static inline int parse_unix_address(php_stream_xport_param *xparam, struct sockaddr_un *unix_addr)
{
	memset(unix_addr, 0, sizeof(*unix_addr));
	unix_addr->sun_family = AF_UNIX;

	/* we need to be binary safe on systems that support an abstract
	 * namespace */
	if (xparam->inputs.namelen >= sizeof(unix_addr->sun_path)) {
		/* On linux, when the path begins with a NUL byte we are
		 * referring to an abstract namespace.  In theory we should
		 * allow an extra byte below, since we don't need the NULL.
		 * BUT, to get into this branch of code, the name is too long,
		 * so we don't care. */
		xparam->inputs.namelen = sizeof(unix_addr->sun_path) - 1;
		php_error_docref(NULL, E_NOTICE,
			"socket path exceeded the maximum allowed length of %lu bytes "
			"and was truncated", (unsigned long)sizeof(unix_addr->sun_path));
	}

	memcpy(unix_addr->sun_path, xparam->inputs.name, xparam->inputs.namelen);

	return 1;
}
