static void init_cfg(int argc, char **argv) {
	EUID_ASSERT();
	memset(&cfg, 0, sizeof(cfg));

	cfg.original_argv = argv;
	cfg.original_argc = argc;
	cfg.bridge0.devsandbox = "eth0";
	cfg.bridge1.devsandbox = "eth1";
	cfg.bridge2.devsandbox = "eth2";
	cfg.bridge3.devsandbox = "eth3";
	
	EUID_ROOT(); // rise permissions for grsecurity
	struct passwd *pw = getpwuid(getuid());
	if (!pw)
		errExit("getpwuid");
	EUID_USER();
	cfg.username = strdup(pw->pw_name);
	if (!cfg.username)
		errExit("strdup");

	cfg.homedir = NULL;
	if (pw->pw_dir != NULL) {
		cfg.homedir = strdup(pw->pw_dir);
		if (!cfg.homedir)
			errExit("strdup");
	}
	else {
		fprintf(stderr, "Error: user %s doesn't have a user directory assigned\n", cfg.username);
		exit(1);
	}
	cfg.cwd = getcwd(NULL, 0);

	sandbox_pid = getpid();
	time_t t = time(NULL);
	srand(t ^ sandbox_pid);
}
