sftp_server_main(int argc, char **argv, struct passwd *user_pw)
{
	fd_set *rset, *wset;
	int i, r, in, out, max, ch, skipargs = 0, log_stderr = 0;
	ssize_t len, olen, set_size;
	SyslogFacility log_facility = SYSLOG_FACILITY_AUTH;
	char *cp, *homedir = NULL, buf[4*4096];
	long mask;

	extern char *optarg;
	extern char *__progname;

	ssh_malloc_init();	/* must be called before any mallocs */
	log_init(__progname, log_level, log_facility, log_stderr);

	pw = pwcopy(user_pw);

	while (!skipargs && (ch = getopt(argc, argv,
	    "d:f:l:P:p:Q:u:cehR")) != -1) {
		switch (ch) {
		case 'Q':
			if (strcasecmp(optarg, "requests") != 0) {
				fprintf(stderr, "Invalid query type\n");
				exit(1);
			}
			for (i = 0; handlers[i].handler != NULL; i++)
				printf("%s\n", handlers[i].name);
			for (i = 0; extended_handlers[i].handler != NULL; i++)
				printf("%s\n", extended_handlers[i].name);
			exit(0);
			break;
		case 'R':
			readonly = 1;
			break;
		case 'c':
			/*
			 * Ignore all arguments if we are invoked as a
			 * shell using "sftp-server -c command"
			 */
			skipargs = 1;
			break;
		case 'e':
			log_stderr = 1;
			break;
		case 'l':
			log_level = log_level_number(optarg);
			if (log_level == SYSLOG_LEVEL_NOT_SET)
				error("Invalid log level \"%s\"", optarg);
			break;
		case 'f':
			log_facility = log_facility_number(optarg);
			if (log_facility == SYSLOG_FACILITY_NOT_SET)
				error("Invalid log facility \"%s\"", optarg);
			break;
		case 'd':
			cp = tilde_expand_filename(optarg, user_pw->pw_uid);
			homedir = percent_expand(cp, "d", user_pw->pw_dir,
			    "u", user_pw->pw_name, (char *)NULL);
			free(cp);
			break;
		case 'p':
			if (request_whitelist != NULL)
				fatal("Permitted requests already set");
			request_whitelist = xstrdup(optarg);
			break;
		case 'P':
			if (request_blacklist != NULL)
				fatal("Refused requests already set");
			request_blacklist = xstrdup(optarg);
			break;
		case 'u':
			errno = 0;
			mask = strtol(optarg, &cp, 8);
			if (mask < 0 || mask > 0777 || *cp != '\0' ||
			    cp == optarg || (mask == 0 && errno != 0))
				fatal("Invalid umask \"%s\"", optarg);
			(void)umask((mode_t)mask);
			break;
		case 'h':
		default:
			sftp_server_usage();
		}
	}

	log_init(__progname, log_level, log_facility, log_stderr);

	if ((cp = getenv("SSH_CONNECTION")) != NULL) {
		client_addr = xstrdup(cp);
		if ((cp = strchr(client_addr, ' ')) == NULL) {
			error("Malformed SSH_CONNECTION variable: \"%s\"",
			    getenv("SSH_CONNECTION"));
			sftp_server_cleanup_exit(255);
		}
		*cp = '\0';
	} else
		client_addr = xstrdup("UNKNOWN");

	logit("session opened for local user %s from [%s]",
	    pw->pw_name, client_addr);

	in = STDIN_FILENO;
	out = STDOUT_FILENO;

	max = 0;
	if (in > max)
		max = in;
	if (out > max)
		max = out;

	if ((iqueue = sshbuf_new()) == NULL)
		fatal("%s: sshbuf_new failed", __func__);
	if ((oqueue = sshbuf_new()) == NULL)
		fatal("%s: sshbuf_new failed", __func__);

	rset = xcalloc(howmany(max + 1, NFDBITS), sizeof(fd_mask));
	wset = xcalloc(howmany(max + 1, NFDBITS), sizeof(fd_mask));

	if (homedir != NULL) {
		if (chdir(homedir) != 0) {
			error("chdir to \"%s\" failed: %s", homedir,
			    strerror(errno));
		}
	}

	set_size = howmany(max + 1, NFDBITS) * sizeof(fd_mask);
	for (;;) {
		memset(rset, 0, set_size);
		memset(wset, 0, set_size);

		/*
		 * Ensure that we can read a full buffer and handle
		 * the worst-case length packet it can generate,
		 * otherwise apply backpressure by stopping reads.
		 */
		if ((r = sshbuf_check_reserve(iqueue, sizeof(buf))) == 0 &&
		    (r = sshbuf_check_reserve(oqueue,
		    SFTP_MAX_MSG_LENGTH)) == 0)
			FD_SET(in, rset);
		else if (r != SSH_ERR_NO_BUFFER_SPACE)
			fatal("%s: sshbuf_check_reserve failed: %s",
			    __func__, ssh_err(r));

		olen = sshbuf_len(oqueue);
		if (olen > 0)
			FD_SET(out, wset);

		if (select(max+1, rset, wset, NULL, NULL) < 0) {
			if (errno == EINTR)
				continue;
			error("select: %s", strerror(errno));
			sftp_server_cleanup_exit(2);
		}

		/* copy stdin to iqueue */
		if (FD_ISSET(in, rset)) {
			len = read(in, buf, sizeof buf);
			if (len == 0) {
				debug("read eof");
				sftp_server_cleanup_exit(0);
			} else if (len < 0) {
				error("read: %s", strerror(errno));
				sftp_server_cleanup_exit(1);
			} else if ((r = sshbuf_put(iqueue, buf, len)) != 0) {
				fatal("%s: buffer error: %s",
				    __func__, ssh_err(r));
			}
		}
		/* send oqueue to stdout */
		if (FD_ISSET(out, wset)) {
			len = write(out, sshbuf_ptr(oqueue), olen);
			if (len < 0) {
				error("write: %s", strerror(errno));
				sftp_server_cleanup_exit(1);
			} else if ((r = sshbuf_consume(oqueue, len)) != 0) {
				fatal("%s: buffer error: %s",
				    __func__, ssh_err(r));
			}
		}

		/*
		 * Process requests from client if we can fit the results
		 * into the output buffer, otherwise stop processing input
		 * and let the output queue drain.
		 */
		r = sshbuf_check_reserve(oqueue, SFTP_MAX_MSG_LENGTH);
		if (r == 0)
			process();
		else if (r != SSH_ERR_NO_BUFFER_SPACE)
			fatal("%s: sshbuf_check_reserve: %s",
			    __func__, ssh_err(r));
	}
}
