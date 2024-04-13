static void uwtmp_login(struct mt_connection *conn) {
#if defined(__FreeBSD__) || defined(__APPLE__)
	struct utmpx utent;
#else
	struct utmp utent;
#endif
	pid_t pid;

	pid = getpid();

	char *line = ttyname(conn->slavefd);
	if (strncmp(line, "/dev/", 5) == 0) {
		line += 5;
	}

	/* Setup utmp struct */
	memset((void *) &utent, 0, sizeof(utent));
	utent.ut_type = USER_PROCESS;
	utent.ut_pid = pid;
	strncpy(utent.ut_user, conn->username, sizeof(utent.ut_user));
	strncpy(utent.ut_line, line, sizeof(utent.ut_line));
	strncpy(utent.ut_id, utent.ut_line + 3, sizeof(utent.ut_id));
	strncpy(utent.ut_host,
                ether_ntoa((const struct ether_addr *)conn->srcmac),
                sizeof(utent.ut_host));
#if defined(__FreeBSD__) || defined(__APPLE__)
	gettimeofday(&utent.ut_tv, NULL);
#else
	time((time_t *)&(utent.ut_time));
#endif

	/* Update utmp and/or wtmp */
#if defined(__FreeBSD__) || defined(__APPLE__)
	setutxent();
	pututxline(&utent);
	endutxent();
#else
	setutent();
	pututline(&utent);
	endutent();
	updwtmp(_PATH_WTMP, &utent);
#endif
}
