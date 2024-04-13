void setup_servers(GArray *const servers, const gchar *const modernaddr,
                   const gchar *const modernport) {
	int i;
	struct sigaction sa;
	int want_modern=0;

	for(i=0;i<servers->len;i++) {
                GError *gerror = NULL;
                SERVER *server = &g_array_index(servers, SERVER, i);
                int ret;

		ret = setup_serve(server, &gerror);
                if (ret == -1) {
                        msg(LOG_ERR, "failed to setup servers: %s",
                            gerror->message);
                        g_clear_error(&gerror);
                        exit(EXIT_FAILURE);
                }
                want_modern |= ret;
	}
	if(want_modern) {
                GError *gerror = NULL;
                if (open_modern(modernaddr, modernport, &gerror) == -1) {
                        msg(LOG_ERR, "failed to setup servers: %s",
                            gerror->message);
                        g_clear_error(&gerror);
                        exit(EXIT_FAILURE);
                }
	}
	children=g_hash_table_new_full(g_int_hash, g_int_equal, NULL, destroy_pid_t);

	sa.sa_handler = sigchld_handler;
	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, SIGTERM);
	sa.sa_flags = SA_RESTART;
	if(sigaction(SIGCHLD, &sa, NULL) == -1)
		err("sigaction: %m");

	sa.sa_handler = sigterm_handler;
	sigemptyset(&sa.sa_mask);
	sigaddset(&sa.sa_mask, SIGCHLD);
	sa.sa_flags = SA_RESTART;
	if(sigaction(SIGTERM, &sa, NULL) == -1)
		err("sigaction: %m");

	sa.sa_handler = sighup_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if(sigaction(SIGHUP, &sa, NULL) == -1)
		err("sigaction: %m");
}
