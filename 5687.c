int main(int argc, char *argv[], char *envp[])
{
	int rv;
	const char *cp;
#ifdef LOGGING_LIBQB
	enum qb_log_target_slot i;
#endif

	init_set_proc_title(argc, argv, envp);
	get_time(&start_time);

	memset(&cl, 0, sizeof(cl));
	strncpy(cl.configfile,
			BOOTH_DEFAULT_CONF, BOOTH_PATH_LEN - 1);
	cl.lockfile[0] = 0;
	debug_level = 0;


	cp = ((cp = strstr(argv[0], ATTR_PROG)) && !strcmp(cp, ATTR_PROG)
		? ATTR_PROG
		: "booth");
#ifndef LOGGING_LIBQB
	cl_log_set_entity(cp);
#else
	qb_log_init(cp, LOG_USER, LOG_DEBUG);  /* prio driven by debug_level */
	for (i = QB_LOG_TARGET_START; i < QB_LOG_TARGET_MAX; i++) {
		if (i == QB_LOG_SYSLOG || i == QB_LOG_BLACKBOX)
			continue;
		qb_log_format_set(i, "%t %H %N: [%P]: %p: %b");
	}
	(void) qb_log_filter_ctl(QB_LOG_STDERR, QB_LOG_FILTER_ADD,
	                         QB_LOG_FILTER_FILE, "*", LOG_DEBUG);
#endif
	cl_log_enable_stderr(TRUE);
	cl_log_set_facility(0);

	rv = read_arguments(argc, argv);
	if (rv < 0)
		goto out;


	switch (cl.type) {
	case STATUS:
		rv = do_status(cl.type);
		break;

	case ARBITRATOR:
	case DAEMON:
	case SITE:
		rv = do_server(cl.type);
		break;

	case CLIENT:
		rv = do_client();
		break;

	case GEOSTORE:
		rv = do_attr();
		break;
	}

out:
#ifdef LOGGING_LIBQB
	qb_log_fini();
#endif
	/* Normalize values. 0x100 would be seen as "OK" by waitpid(). */
	return (rv >= 0 && rv < 0x70) ? rv : 1;
}