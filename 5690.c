static int do_server(int type)
{
	int rv = -1;
	static char log_ent[128] = DAEMON_NAME "-";

	rv = setup_config(type);
	if (rv < 0)
		return rv;

	if (!local) {
		log_error("Cannot find myself in the configuration.");
		exit(EXIT_FAILURE);
	}

	if (daemonize) {
		if (daemon(0, 0) < 0) {
			perror("daemon error");
			exit(EXIT_FAILURE);
		}
	}

	/*
	 * Register signal and exit handler
	 */
	signal(SIGUSR1, (__sighandler_t)sig_usr1_handler);
	signal(SIGTERM, (__sighandler_t)sig_exit_handler);
	signal(SIGINT, (__sighandler_t)sig_exit_handler);
	/* we'll handle errors there and then */
	signal(SIGPIPE, SIG_IGN);

	atexit(server_exit);

	/* The lockfile must be written to _after_ the call to daemon(), so
	 * that the lockfile contains the pid of the daemon, not the parent. */
	lock_fd = create_lockfile();
	if (lock_fd < 0)
		return lock_fd;

	strcat(log_ent, type_to_string(local->type));
	cl_log_set_entity(log_ent);
	cl_log_enable_stderr(enable_stderr ? TRUE : FALSE);
	cl_log_set_facility(HA_LOG_FACILITY);
	cl_inherit_logging_environment(0);

	log_info("BOOTH %s %s daemon is starting",
			type_to_string(local->type), RELEASE_STR);


	set_scheduler();
	/* we don't want to be killed by the OOM-killer */
	if (set_procfs_val("/proc/self/oom_score_adj", "-999"))
		(void)set_procfs_val("/proc/self/oom_adj", "-16");
	set_proc_title("%s %s %s for [%s]:%d",
			DAEMON_NAME,
			cl.configfile,
			type_to_string(local->type),
			local->addr_string,
			booth_conf->port);

	rv = limit_this_process();
	if (rv)
		return rv;

#ifdef COREDUMP_NURSING
	if (cl_enable_coredumps(TRUE) < 0){
		log_error("enabling core dump failed");
	}
	cl_cdtocoredir();
	prctl(PR_SET_DUMPABLE, (unsigned long)TRUE, 0UL, 0UL, 0UL);
#else
	if (chdir(BOOTH_CORE_DIR) < 0) {
		log_error("cannot change working directory to %s", BOOTH_CORE_DIR);
	}
#endif

	signal(SIGCHLD, (__sighandler_t)sig_chld_handler);
	rv = loop(lock_fd);

	return rv;
}