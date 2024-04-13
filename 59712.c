int main(int argc, char **argv) {
	int i;
	int prog_index = -1;			  // index in argv where the program command starts
	int lockfd = -1;
	int option_cgroup = 0;
	int option_force = 0;
	int custom_profile = 0;	// custom profile loaded
	char *custom_profile_dir = NULL; // custom profile directory
	int arg_noprofile = 0; // use default.profile if none other found/specified
#ifdef HAVE_SECCOMP
	int highest_errno = errno_highest_nr();
#endif

	detect_quiet(argc, argv);
	detect_allow_debuggers(argc, argv);

	EUID_INIT();
	EUID_USER();


	if (*argv[0] != '-')
		run_symlink(argc, argv);

	if (check_namespace_virt() == 0) {
		EUID_ROOT();
		int rv = check_kernel_procs();
		EUID_USER();
		if (rv == 0) {
			int found = 0;
			for (i = 1; i < argc; i++) {
				if (strcmp(argv[i], "--force") == 0 ||
				    strcmp(argv[i], "--list") == 0 ||	
				    strcmp(argv[i], "--netstats") == 0 ||	
				    strcmp(argv[i], "--tree") == 0 ||	
				    strcmp(argv[i], "--top") == 0 ||
				    strncmp(argv[i], "--ls=", 5) == 0 ||
				    strncmp(argv[i], "--get=", 6) == 0 ||
				    strcmp(argv[i], "--debug-caps") == 0 ||
				    strcmp(argv[i], "--debug-errnos") == 0 ||
				    strcmp(argv[i], "--debug-syscalls") == 0 ||
				    strcmp(argv[i], "--debug-protocols") == 0 ||
				    strcmp(argv[i], "--help") == 0 ||
				    strcmp(argv[i], "--version") == 0 ||
				    strcmp(argv[i], "--overlay-clean") == 0 ||
				    strncmp(argv[i], "--dns.print=", 12) == 0 ||
				    strncmp(argv[i], "--bandwidth=", 12) == 0 ||
				    strncmp(argv[i], "--caps.print=", 13) == 0 ||
				    strncmp(argv[i], "--cpu.print=", 12) == 0 ||
				    strncmp(argv[i], "--join=", 7) == 0 ||
	
				    strncmp(argv[i], "--join-filesystem=", 18) == 0 ||
				    strncmp(argv[i], "--join-network=", 15) == 0 ||
				    strncmp(argv[i], "--fs.print=", 11) == 0 ||
				    strncmp(argv[i], "--protocol.print=", 17) == 0 ||
				    strncmp(argv[i], "--seccomp.print", 15) == 0 ||
				    strncmp(argv[i], "--shutdown=", 11) == 0) {
					found = 1;
					break;
				}
	
				if (strcmp(argv[i], "--") == 0)
					break;
				if (strncmp(argv[i], "--", 2) != 0)
					break;
			}
			
			if (found == 0) {
				run_no_sandbox(argc, argv);
				assert(0);
			}
			else
				option_force = 1;
		}
	}
	
	EUID_ROOT();
	if (geteuid()) {
		for (i = 1; i < argc; i++) {
			if (strcmp(argv[i], "--version") == 0) {
				printf("firejail version %s\n", VERSION);
				exit(0);
			}
			
			if (strcmp(argv[i], "--") == 0)
				break;
			if (strncmp(argv[i], "--", 2) != 0)
				break;
		}
		exit(1);
	}
	EUID_USER();

	init_cfg(argc, argv);


	EUID_ROOT();
	fs_build_firejail_dir();
	bandwidth_del_run_file(sandbox_pid);
	network_del_run_file(sandbox_pid);
	delete_name_file(sandbox_pid);
	delete_x11_file(sandbox_pid);
	
	EUID_USER();
	
	int parent_sshd = 0;
	{
		pid_t ppid = getppid();
		EUID_ROOT();
		char *comm = pid_proc_comm(ppid);
		EUID_USER();
		if (comm) {
			if (strcmp(comm, "sshd") == 0) {
				arg_quiet = 1;
				parent_sshd = 1;

#ifdef DEBUG_RESTRICTED_SHELL
				{EUID_ROOT();
				FILE *fp = fopen("/firelog", "w");
				if (fp) {
					int i;
					fprintf(fp, "argc %d: ", argc);
					for (i = 0; i < argc; i++)
						fprintf(fp, "#%s# ", argv[i]);
					fprintf(fp, "\n");
					fclose(fp);
				}
				EUID_USER();}
#endif
				if (*argv[0] != '-') {
					if (strcmp(argv[1], "-c") == 0 && argc > 2) {
						if (strcmp(argv[2], "/usr/lib/openssh/sftp-server") == 0 ||
						    strncmp(argv[2], "scp ", 4) == 0) {
#ifdef DEBUG_RESTRICTED_SHELL
							{EUID_ROOT();
							FILE *fp = fopen("/firelog", "a");
							if (fp) {
								fprintf(fp, "run without a sandbox\n");
								fclose(fp);
							}
							EUID_USER();}
#endif
						    
						    	drop_privs(1);
						    	int rv = system(argv[2]);
						    	exit(rv);
						}
					}
				}
			}
			free(comm);
		}
	}
	
	if (*argv[0] == '-' || parent_sshd) {
		if (argc == 1)
			login_shell = 1;
		fullargc = restricted_shell(cfg.username);
		if (fullargc) {
			
#ifdef DEBUG_RESTRICTED_SHELL
			{EUID_ROOT();
			FILE *fp = fopen("/firelog", "a");
			if (fp) {
				fprintf(fp, "fullargc %d: ",  fullargc);
				int i;
				for (i = 0; i < fullargc; i++)
					fprintf(fp, "#%s# ", fullargv[i]);
				fprintf(fp, "\n");
				fclose(fp);
			}
			EUID_USER();}
#endif
			
			int j;
			for (i = 1, j = fullargc; i < argc && j < MAX_ARGS; i++, j++, fullargc++)
				fullargv[j] = argv[i];

			argv = fullargv;
			argc = j;

#ifdef DEBUG_RESTRICTED_SHELL
			{EUID_ROOT();
			FILE *fp = fopen("/firelog", "a");
			if (fp) {
				fprintf(fp, "argc %d: ", argc);
				int i;
				for (i = 0; i < argc; i++)
					fprintf(fp, "#%s# ", argv[i]);
				fprintf(fp, "\n");
				fclose(fp);
			}
			EUID_USER();}
#endif
		}
	}
	else {
		check_output(argc, argv); // the function will not return if --output option was found
	}
	
	
	if (checkcfg(CFG_FORCE_NONEWPRIVS))
		arg_nonewprivs = 1;
	
	if (arg_allow_debuggers) {
		char *cmd = strdup("noblacklist ${PATH}/strace");
		if (!cmd)
			errExit("strdup");
		profile_add(cmd);
	}
	
	for (i = 1; i < argc; i++) {
		run_cmd_and_exit(i, argc, argv); // will exit if the command is recognized
		
		if (strcmp(argv[i], "--debug") == 0) {
			if (!arg_quiet) {
				arg_debug = 1;
				if (option_force)
					printf("Entering sandbox-in-sandbox mode\n");
			}
		}
		else if (strcmp(argv[i], "--debug-check-filename") == 0)
			arg_debug_check_filename = 1;
		else if (strcmp(argv[i], "--debug-blacklists") == 0)
			arg_debug_blacklists = 1;
		else if (strcmp(argv[i], "--debug-whitelists") == 0)
			arg_debug_whitelists = 1;
		else if (strcmp(argv[i], "--quiet") == 0) {
			arg_quiet = 1;
			arg_debug = 0;
		}
		else if (strcmp(argv[i], "--force") == 0)
			;
		else if (strcmp(argv[i], "--allow-debuggers") == 0) {
		}

#ifdef HAVE_APPARMOR
		else if (strcmp(argv[i], "--apparmor") == 0)
			arg_apparmor = 1;
#endif	
#ifdef HAVE_SECCOMP
		else if (strncmp(argv[i], "--protocol=", 11) == 0) {
			if (checkcfg(CFG_SECCOMP)) {
				protocol_store(argv[i] + 11);
			}
			else {
				fprintf(stderr, "Error: seccomp feature is disabled in Firejail configuration file\n");
				exit(1);
			}
		}
		else if (strcmp(argv[i], "--seccomp") == 0) {
			if (checkcfg(CFG_SECCOMP)) {
				if (arg_seccomp) {
					fprintf(stderr, "Error: seccomp already enabled\n");
					exit(1);
				}
				arg_seccomp = 1;
			}
			else {
				fprintf(stderr, "Error: seccomp feature is disabled in Firejail configuration file\n");
				exit(1);
			}
		}
		else if (strncmp(argv[i], "--seccomp=", 10) == 0) {
			if (checkcfg(CFG_SECCOMP)) {
				if (arg_seccomp) {
					fprintf(stderr, "Error: seccomp already enabled\n");
					exit(1);
				}
				arg_seccomp = 1;
				cfg.seccomp_list = strdup(argv[i] + 10);
				if (!cfg.seccomp_list)
					errExit("strdup");
			}
			else {
				fprintf(stderr, "Error: seccomp feature is disabled in Firejail configuration file\n");
				exit(1);
			}
		}
		else if (strncmp(argv[i], "--seccomp.drop=", 15) == 0) {
			if (checkcfg(CFG_SECCOMP)) {
				if (arg_seccomp) {
					fprintf(stderr, "Error: seccomp already enabled\n");
					exit(1);
				}
				arg_seccomp = 1;
				cfg.seccomp_list_drop = strdup(argv[i] + 15);
				if (!cfg.seccomp_list_drop)
					errExit("strdup");
			}
			else {
				fprintf(stderr, "Error: seccomp feature is disabled in Firejail configuration file\n");
				exit(1);
			}
		}
		else if (strncmp(argv[i], "--seccomp.keep=", 15) == 0) {
			if (checkcfg(CFG_SECCOMP)) {
				if (arg_seccomp) {
					fprintf(stderr, "Error: seccomp already enabled\n");
					exit(1);
				}
				arg_seccomp = 1;
				cfg.seccomp_list_keep = strdup(argv[i] + 15);
				if (!cfg.seccomp_list_keep)
					errExit("strdup");
			}
			else {
				fprintf(stderr, "Error: seccomp feature is disabled in Firejail configuration file\n");
				exit(1);
			}
		}
		else if (strncmp(argv[i], "--seccomp.e", 11) == 0 && strchr(argv[i], '=')) {
			if (checkcfg(CFG_SECCOMP)) {
				if (arg_seccomp && !cfg.seccomp_list_errno) {
					fprintf(stderr, "Error: seccomp already enabled\n");
					exit(1);
				}
				char *eq = strchr(argv[i], '=');
				char *errnoname = strndup(argv[i] + 10, eq - (argv[i] + 10));
				int nr = errno_find_name(errnoname);
				if (nr == -1) {
					fprintf(stderr, "Error: unknown errno %s\n", errnoname);
					free(errnoname);
					exit(1);
				}
	
				if (!cfg.seccomp_list_errno)
					cfg.seccomp_list_errno = calloc(highest_errno+1, sizeof(cfg.seccomp_list_errno[0]));
	
				if (cfg.seccomp_list_errno[nr]) {
					fprintf(stderr, "Error: errno %s already configured\n", errnoname);
					free(errnoname);
					exit(1);
				}
				arg_seccomp = 1;
				cfg.seccomp_list_errno[nr] = strdup(eq+1);
				if (!cfg.seccomp_list_errno[nr])
					errExit("strdup");
				free(errnoname);
			}
			else {
				fprintf(stderr, "Error: seccomp feature is disabled in Firejail configuration file\n");
				exit(1);
			}
		}
#endif		
		else if (strcmp(argv[i], "--caps") == 0)
			arg_caps_default_filter = 1;
		else if (strcmp(argv[i], "--caps.drop=all") == 0)
			arg_caps_drop_all = 1;
		else if (strncmp(argv[i], "--caps.drop=", 12) == 0) {
			arg_caps_drop = 1;
			arg_caps_list = strdup(argv[i] + 12);
			if (!arg_caps_list)
				errExit("strdup");
			if (caps_check_list(arg_caps_list, NULL))
				return 1;
		}
		else if (strncmp(argv[i], "--caps.keep=", 12) == 0) {
			arg_caps_keep = 1;
			arg_caps_list = strdup(argv[i] + 12);
			if (!arg_caps_list)
				errExit("strdup");
			if (caps_check_list(arg_caps_list, NULL))
				return 1;
		}


		else if (strcmp(argv[i], "--trace") == 0)
			arg_trace = 1;
		else if (strcmp(argv[i], "--tracelog") == 0)
			arg_tracelog = 1;
		else if (strncmp(argv[i], "--rlimit-nofile=", 16) == 0) {
			if (not_unsigned(argv[i] + 16)) {
				fprintf(stderr, "Error: invalid rlimt nofile\n");
				exit(1);
			}
			sscanf(argv[i] + 16, "%u", &cfg.rlimit_nofile);
			arg_rlimit_nofile = 1;
		}		
		else if (strncmp(argv[i], "--rlimit-nproc=", 15) == 0) {
			if (not_unsigned(argv[i] + 15)) {
				fprintf(stderr, "Error: invalid rlimt nproc\n");
				exit(1);
			}
			sscanf(argv[i] + 15, "%u", &cfg.rlimit_nproc);
			arg_rlimit_nproc = 1;
		}	
		else if (strncmp(argv[i], "--rlimit-fsize=", 15) == 0) {
			if (not_unsigned(argv[i] + 15)) {
				fprintf(stderr, "Error: invalid rlimt fsize\n");
				exit(1);
			}
			sscanf(argv[i] + 15, "%u", &cfg.rlimit_fsize);
			arg_rlimit_fsize = 1;
		}	
		else if (strncmp(argv[i], "--rlimit-sigpending=", 20) == 0) {
			if (not_unsigned(argv[i] + 20)) {
				fprintf(stderr, "Error: invalid rlimt sigpending\n");
				exit(1);
			}
			sscanf(argv[i] + 20, "%u", &cfg.rlimit_sigpending);
			arg_rlimit_sigpending = 1;
		}	
		else if (strncmp(argv[i], "--ipc-namespace", 15) == 0)
			arg_ipc = 1;
		else if (strncmp(argv[i], "--cpu=", 6) == 0)
			read_cpu_list(argv[i] + 6);
		else if (strncmp(argv[i], "--nice=", 7) == 0) {
			cfg.nice = atoi(argv[i] + 7);
			if (getuid() != 0 &&cfg.nice < 0)
				cfg.nice = 0;
			arg_nice = 1;
		}
		else if (strncmp(argv[i], "--cgroup=", 9) == 0) {
			if (option_cgroup) {
				fprintf(stderr, "Error: only a cgroup can be defined\n");
				exit(1);
			}
			
			option_cgroup = 1;
			cfg.cgroup = strdup(argv[i] + 9);
			if (!cfg.cgroup)
				errExit("strdup");
			set_cgroup(cfg.cgroup);
		}
		
		else if (strcmp(argv[i], "--allusers") == 0)
			arg_allusers = 1;
#ifdef HAVE_BIND		
		else if (strncmp(argv[i], "--bind=", 7) == 0) {
			if (checkcfg(CFG_BIND)) {
				char *line;
				if (asprintf(&line, "bind %s", argv[i] + 7) == -1)
					errExit("asprintf");
	
				profile_check_line(line, 0, NULL);	// will exit if something wrong
				profile_add(line);
			}
			else {
				fprintf(stderr, "Error: --bind feature is disabled in Firejail configuration file\n");
				exit(1);
			}
		}
#endif
		else if (strncmp(argv[i], "--tmpfs=", 8) == 0) {
			char *line;
			if (asprintf(&line, "tmpfs %s", argv[i] + 8) == -1)
				errExit("asprintf");
			
			profile_check_line(line, 0, NULL);	// will exit if something wrong
			profile_add(line);
		}
		else if (strncmp(argv[i], "--blacklist=", 12) == 0) {
			char *line;
			if (asprintf(&line, "blacklist %s", argv[i] + 12) == -1)
				errExit("asprintf");
			
			profile_check_line(line, 0, NULL);	// will exit if something wrong
			profile_add(line);
		}
		else if (strncmp(argv[i], "--noblacklist=", 14) == 0) {
			char *line;
			if (asprintf(&line, "noblacklist %s", argv[i] + 14) == -1)
				errExit("asprintf");
			
			profile_check_line(line, 0, NULL);	// will exit if something wrong
			profile_add(line);
		}

#ifdef HAVE_WHITELIST
		else if (strncmp(argv[i], "--whitelist=", 12) == 0) {
			if (checkcfg(CFG_WHITELIST)) {
				char *line;
				if (asprintf(&line, "whitelist %s", argv[i] + 12) == -1)
					errExit("asprintf");
				
				profile_check_line(line, 0, NULL);	// will exit if something wrong
				profile_add(line);
			}
			else {
				fprintf(stderr, "Error: whitelist feature is disabled in Firejail configuration file\n");
				exit(1);
			}
		}
#endif		

		else if (strncmp(argv[i], "--read-only=", 12) == 0) {
			char *line;
			if (asprintf(&line, "read-only %s", argv[i] + 12) == -1)
				errExit("asprintf");
			
			profile_check_line(line, 0, NULL);	// will exit if something wrong
			profile_add(line);
		}
		else if (strncmp(argv[i], "--noexec=", 9) == 0) {
			char *line;
			if (asprintf(&line, "noexec %s", argv[i] + 9) == -1)
				errExit("asprintf");
			
			profile_check_line(line, 0, NULL);	// will exit if something wrong
			profile_add(line);
		}
		else if (strncmp(argv[i], "--read-write=", 13) == 0) {
			char *line;
			if (asprintf(&line, "read-write %s", argv[i] + 13) == -1)
				errExit("asprintf");
			
			profile_check_line(line, 0, NULL);	// will exit if something wrong
			profile_add(line);
		}
#ifdef HAVE_OVERLAYFS
		else if (strcmp(argv[i], "--overlay") == 0) {
			if (checkcfg(CFG_OVERLAYFS)) {
				if (cfg.chrootdir) {
					fprintf(stderr, "Error: --overlay and --chroot options are mutually exclusive\n");
					exit(1);
				}
				struct stat s;
				if (stat("/proc/sys/kernel/grsecurity", &s) == 0) {
					fprintf(stderr, "Error: --overlay option is not available on Grsecurity systems\n");
					exit(1);	
				}
				arg_overlay = 1;
				arg_overlay_keep = 1;
				
				char *subdirname;
				if (asprintf(&subdirname, "%d", getpid()) == -1)
					errExit("asprintf");
				cfg.overlay_dir = fs_check_overlay_dir(subdirname, arg_overlay_reuse);
	
				free(subdirname);
			}
			else {
				fprintf(stderr, "Error: overlayfs feature is disabled in Firejail configuration file\n");
				exit(1);
			}
		}
		else if (strncmp(argv[i], "--overlay-named=", 16) == 0) {
			if (checkcfg(CFG_OVERLAYFS)) {
				if (cfg.chrootdir) {
					fprintf(stderr, "Error: --overlay and --chroot options are mutually exclusive\n");
					exit(1);
				}
				struct stat s;
				if (stat("/proc/sys/kernel/grsecurity", &s) == 0) {
					fprintf(stderr, "Error: --overlay option is not available on Grsecurity systems\n");
					exit(1);
				}
				arg_overlay = 1;
				arg_overlay_keep = 1;
				arg_overlay_reuse = 1;
				
				char *subdirname = argv[i] + 16;
				if (subdirname == '\0') {
					fprintf(stderr, "Error: invalid overlay option\n");
					exit(1);
				}
				
				invalid_filename(subdirname);
				if (strstr(subdirname, "..") || strstr(subdirname, "/")) {
					fprintf(stderr, "Error: invalid overlay name\n");
					exit(1);
				}
				cfg.overlay_dir = fs_check_overlay_dir(subdirname, arg_overlay_reuse);
			}
			else {
				fprintf(stderr, "Error: overlayfs feature is disabled in Firejail configuration file\n");
				exit(1);
			}

		}
#if 0 // disabled for now, it could be used to overwrite system directories	
		else if (strncmp(argv[i], "--overlay-path=", 15) == 0) {
			if (checkcfg(CFG_OVERLAYFS)) {
				if (cfg.chrootdir) {
					fprintf(stderr, "Error: --overlay and --chroot options are mutually exclusive\n");
					exit(1);
				}
				struct stat s;
				if (stat("/proc/sys/kernel/grsecurity", &s) == 0) {
					fprintf(stderr, "Error: --overlay option is not available on Grsecurity systems\n");
					exit(1);
				}
				arg_overlay = 1;
				arg_overlay_keep = 1;
				arg_overlay_reuse = 1;
				
				char *dirname = argv[i] + 15;
				if (dirname == '\0') {
					fprintf(stderr, "Error: invalid overlay option\n");
					exit(1);
				}
				cfg.overlay_dir = expand_home(dirname, cfg.homedir);
			}
			else {
				fprintf(stderr, "Error: overlayfs feature is disabled in Firejail configuration file\n");
				exit(1);
			}
		}
#endif
		else if (strcmp(argv[i], "--overlay-tmpfs") == 0) {
			if (checkcfg(CFG_OVERLAYFS)) {
				if (cfg.chrootdir) {
					fprintf(stderr, "Error: --overlay and --chroot options are mutually exclusive\n");
					exit(1);
				}
				struct stat s;
				if (stat("/proc/sys/kernel/grsecurity", &s) == 0) {
					fprintf(stderr, "Error: --overlay option is not available on Grsecurity systems\n");
					exit(1);	
				}
				arg_overlay = 1;
			}
			else {
				fprintf(stderr, "Error: overlayfs feature is disabled in Firejail configuration file\n");
				exit(1);
			}
		}
#endif
		else if (strncmp(argv[i], "--profile=", 10) == 0) {
			if (arg_noprofile) {
				fprintf(stderr, "Error: --noprofile and --profile options are mutually exclusive\n");
				exit(1);
			}
			
			char *ppath = expand_home(argv[i] + 10, cfg.homedir);
			if (!ppath)
				errExit("strdup");
			invalid_filename(ppath);
			
			if (is_dir(ppath) || is_link(ppath) || strstr(ppath, "..")) {
				fprintf(stderr, "Error: invalid profile file\n");
				exit(1);
			}
			
			if (access(ppath, R_OK)) {
				fprintf(stderr, "Error: cannot access profile file\n");
				return 1;
			}

			profile_read(ppath);
			custom_profile = 1;
			free(ppath);
		}
		else if (strncmp(argv[i], "--profile-path=", 15) == 0) {
			if (arg_noprofile) {
				fprintf(stderr, "Error: --noprofile and --profile-path options are mutually exclusive\n");
				exit(1);
			}
			custom_profile_dir = expand_home(argv[i] + 15, cfg.homedir);
			invalid_filename(custom_profile_dir);
			if (!is_dir(custom_profile_dir) || is_link(custom_profile_dir) || strstr(custom_profile_dir, "..")) {
				fprintf(stderr, "Error: invalid profile path\n");
				exit(1);
			}
			
			if (access(custom_profile_dir, R_OK)) {
				fprintf(stderr, "Error: cannot access profile directory\n");
				return 1;
			}
		}
		else if (strcmp(argv[i], "--noprofile") == 0) {
			if (custom_profile) {
				fprintf(stderr, "Error: --profile and --noprofile options are mutually exclusive\n");
				exit(1);
			}
			arg_noprofile = 1;
		}
		else if (strncmp(argv[i], "--ignore=", 9) == 0) {
			if (custom_profile) {
				fprintf(stderr, "Error: please use --profile after --ignore\n");
				exit(1);
			}

			if (*(argv[i] + 9) == '\0') {
				fprintf(stderr, "Error: invalid ignore option\n");
				exit(1);
			}
			
			int j;
			for (j = 0; j < MAX_PROFILE_IGNORE; j++) {
				if (cfg.profile_ignore[j] == NULL) 
					break;
			}
			if (j >= MAX_PROFILE_IGNORE) {
				fprintf(stderr, "Error: maximum %d --ignore options are permitted\n", MAX_PROFILE_IGNORE);
				exit(1);
			}
			else
				cfg.profile_ignore[j] = argv[i] + 9;
		}
#ifdef HAVE_CHROOT		
		else if (strncmp(argv[i], "--chroot=", 9) == 0) {
			if (checkcfg(CFG_CHROOT)) {
				if (arg_overlay) {
					fprintf(stderr, "Error: --overlay and --chroot options are mutually exclusive\n");
					exit(1);
				}
				
				struct stat s;
				if (stat("/proc/sys/kernel/grsecurity", &s) == 0) {
					fprintf(stderr, "Error: --chroot option is not available on Grsecurity systems\n");
					exit(1);	
				}
				
				
				invalid_filename(argv[i] + 9);
				
				cfg.chrootdir = argv[i] + 9;
				if (*cfg.chrootdir == '~') {
					char *tmp;
					if (asprintf(&tmp, "%s%s", cfg.homedir, cfg.chrootdir + 1) == -1)
						errExit("asprintf");
					cfg.chrootdir = tmp;
				}
				
				if (strstr(cfg.chrootdir, "..") || !is_dir(cfg.chrootdir) || is_link(cfg.chrootdir)) {
					fprintf(stderr, "Error: invalid directory %s\n", cfg.chrootdir);
					return 1;
				}
				
				char *rpath = realpath(cfg.chrootdir, NULL);
				if (rpath == NULL || strcmp(rpath, "/") == 0) {
					fprintf(stderr, "Error: invalid chroot directory\n");
					exit(1);
				}
				free(rpath);
				
				if (fs_check_chroot_dir(cfg.chrootdir)) {
					fprintf(stderr, "Error: invalid chroot\n");
					exit(1);
				}
			}
			else {
				fprintf(stderr, "Error: --chroot feature is disabled in Firejail configuration file\n");
				exit(1);
			}
		}
#endif
		else if (strcmp(argv[i], "--writable-etc") == 0) {
			if (cfg.etc_private_keep) {
				fprintf(stderr, "Error: --private-etc and --writable-etc are mutually exclusive\n");
				exit(1);
			}
			arg_writable_etc = 1;
		}
		else if (strcmp(argv[i], "--writable-var") == 0) {
			arg_writable_var = 1;
		}
		else if (strcmp(argv[i], "--private") == 0) {
			arg_private = 1;
		}
		else if (strncmp(argv[i], "--private=", 10) == 0) {
			if (cfg.home_private_keep) {
				fprintf(stderr, "Error: a private list of files was already defined with --private-home option.\n");
				exit(1);
			}

			cfg.home_private = argv[i] + 10;
			if (*cfg.home_private == '\0') {
				fprintf(stderr, "Error: invalid private option\n");
				exit(1);
			}
			fs_check_private_dir();

			if (strcmp(cfg.home_private, cfg.homedir) == 0) {
				free(cfg.home_private);
				cfg.home_private = NULL;
			}
			arg_private = 1;
		}
#ifdef HAVE_PRIVATE_HOME
		else if (strncmp(argv[i], "--private-home=", 15) == 0) {
			if (checkcfg(CFG_PRIVATE_HOME)) {
				if (cfg.home_private) {
					fprintf(stderr, "Error: a private home directory was already defined with --private option.\n");
					exit(1);
				}
				
				cfg.home_private_keep = argv[i] + 15;
				fs_check_home_list();
				arg_private = 1;
			}
			else {
				fprintf(stderr, "Error: --private-home feature is disabled in Firejail configuration file\n");
				exit(1);
			}
		}
#endif		
		else if (strcmp(argv[i], "--private-dev") == 0) {
			arg_private_dev = 1;
		}
		else if (strncmp(argv[i], "--private-etc=", 14) == 0) {
			if (arg_writable_etc) {
				fprintf(stderr, "Error: --private-etc and --writable-etc are mutually exclusive\n");
				exit(1);
			}
			
			cfg.etc_private_keep = argv[i] + 14;
			if (*cfg.etc_private_keep == '\0') {
				fprintf(stderr, "Error: invalid private-etc option\n");
				exit(1);
			}
			fs_check_etc_list();
			arg_private_etc = 1;
		}
		else if (strncmp(argv[i], "--private-bin=", 14) == 0) {
			cfg.bin_private_keep = argv[i] + 14;
			if (*cfg.bin_private_keep == '\0') {
				fprintf(stderr, "Error: invalid private-bin option\n");
				exit(1);
			}
			arg_private_bin = 1;
			fs_check_bin_list();
		}
		else if (strcmp(argv[i], "--private-tmp") == 0) {
			arg_private_tmp = 1;
		}

		else if (strncmp(argv[i], "--name=", 7) == 0) {
			cfg.name = argv[i] + 7;
			if (strlen(cfg.name) == 0) {
				fprintf(stderr, "Error: please provide a name for sandbox\n");
				return 1;
			}
		}
		else if (strncmp(argv[i], "--hostname=", 11) == 0) {
			cfg.hostname = argv[i] + 11;
			if (strlen(cfg.hostname) == 0) {
				fprintf(stderr, "Error: please provide a hostname for sandbox\n");
				return 1;
			}
		}
		else if (strcmp(argv[i], "--nogroups") == 0)
			arg_nogroups = 1;
#ifdef HAVE_USERNS
		else if (strcmp(argv[i], "--noroot") == 0) {
			if (checkcfg(CFG_USERNS))
				check_user_namespace();
			else {
				fprintf(stderr, "Error: --noroot feature is disabled in Firejail configuration file\n");
				exit(1);
			}
		}
#endif
		else if (strcmp(argv[i], "--nonewprivs") == 0) {
			arg_nonewprivs = 1;
		}
		else if (strncmp(argv[i], "--env=", 6) == 0)
			env_store(argv[i] + 6, SETENV);
		else if (strncmp(argv[i], "--rmenv=", 8) == 0)
			env_store(argv[i] + 8, RMENV);
		else if (strcmp(argv[i], "--nosound") == 0) {
			arg_nosound = 1;
		}
		else if (strcmp(argv[i], "--no3d") == 0) {
			arg_no3d = 1;
		}
				
#ifdef HAVE_NETWORK	
		else if (strncmp(argv[i], "--interface=", 12) == 0) {
			if (checkcfg(CFG_NETWORK)) {
#ifdef HAVE_NETWORK_RESTRICTED
				if (getuid() != 0) {
					fprintf(stderr, "Error: --interface is allowed only to root user\n");
					exit(1);
				}
#endif
				if (checkcfg(CFG_RESTRICTED_NETWORK) && getuid() != 0) {
					fprintf(stderr, "Error: --interface is allowed only to root user\n");
					exit(1);
				}
		
				if (arg_nonetwork) {
					fprintf(stderr, "Error: --network=none and --interface are incompatible\n");
					exit(1);
				}
				if (strcmp(argv[i] + 12, "lo") == 0) {
					fprintf(stderr, "Error: cannot use lo device in --interface command\n");
					exit(1);
				}
				int ifindex = if_nametoindex(argv[i] + 12);
				if (ifindex <= 0) {
					fprintf(stderr, "Error: cannot find interface %s\n", argv[i] + 12);
					exit(1);
				}
				
				Interface *intf;
				if (cfg.interface0.configured == 0)
					intf = &cfg.interface0;
				else if (cfg.interface1.configured == 0)
					intf = &cfg.interface1;
				else if (cfg.interface2.configured == 0)
					intf = &cfg.interface2;
				else if (cfg.interface3.configured == 0)
					intf = &cfg.interface3;
				else {
					fprintf(stderr, "Error: maximum 4 interfaces are allowed\n");
					return 1;
				}
				
				intf->dev = strdup(argv[i] + 12);
				if (!intf->dev)
					errExit("strdup");
				
				if (net_get_if_addr(intf->dev, &intf->ip, &intf->mask, intf->mac, &intf->mtu)) {
					if (!arg_quiet || arg_debug)
						fprintf(stderr, "Warning:  interface %s is not configured\n", intf->dev);
				}
				intf->configured = 1;
			}
			else {
				fprintf(stderr, "Error: networking features are disabled in Firejail configuration file\n");
				exit(1);
			}
		}

		else if (strncmp(argv[i], "--net=", 6) == 0) {
			if (checkcfg(CFG_NETWORK)) {
				if (strcmp(argv[i] + 6, "none") == 0) {
					arg_nonetwork  = 1;
					cfg.bridge0.configured = 0;
					cfg.bridge1.configured = 0;
					cfg.bridge2.configured = 0;
					cfg.bridge3.configured = 0;
					cfg.interface0.configured = 0;
					cfg.interface1.configured = 0;
					cfg.interface2.configured = 0;
					cfg.interface3.configured = 0;
					continue;
				}

#ifdef HAVE_NETWORK_RESTRICTED
				if (getuid() != 0) {
					fprintf(stderr, "Error: only --net=none is allowed to non-root users\n");
					exit(1);
				}
#endif
				if (checkcfg(CFG_RESTRICTED_NETWORK) && getuid() != 0) {
					fprintf(stderr, "Error: only --net=none is allowed to non-root users\n");
					exit(1);
				}
				if (strcmp(argv[i] + 6, "lo") == 0) {
					fprintf(stderr, "Error: cannot attach to lo device\n");
					exit(1);
				}
	
				Bridge *br;
				if (cfg.bridge0.configured == 0)
					br = &cfg.bridge0;
				else if (cfg.bridge1.configured == 0)
					br = &cfg.bridge1;
				else if (cfg.bridge2.configured == 0)
					br = &cfg.bridge2;
				else if (cfg.bridge3.configured == 0)
					br = &cfg.bridge3;
				else {
					fprintf(stderr, "Error: maximum 4 network devices are allowed\n");
					return 1;
				}
				net_configure_bridge(br, argv[i] + 6);
			}
			else {
				fprintf(stderr, "Error: networking features are disabled in Firejail configuration file\n");
				exit(1);
			}
		}

		else if (strncmp(argv[i], "--veth-name=", 12) == 0) {
			if (checkcfg(CFG_NETWORK)) {
				Bridge *br = last_bridge_configured();
				if (br == NULL) {
					fprintf(stderr, "Error: no network device configured\n");
					exit(1);
				}
				br->veth_name = strdup(argv[i] + 12);
				if (br->veth_name == NULL)
					errExit("strdup");
				if (*br->veth_name == '\0') {
					fprintf(stderr, "Error: no veth-name configured\n");
					exit(1);
				}
			}
			else {
				fprintf(stderr, "Error: networking features are disabled in Firejail configuration file\n");
				exit(1);
			}
		}

		else if (strcmp(argv[i], "--scan") == 0) {
			if (checkcfg(CFG_NETWORK)) {
				arg_scan = 1;
			}
			else {
				fprintf(stderr, "Error: networking features are disabled in Firejail configuration file\n");
				exit(1);
			}
		}
		else if (strncmp(argv[i], "--iprange=", 10) == 0) {
			if (checkcfg(CFG_NETWORK)) {
				Bridge *br = last_bridge_configured();
				if (br == NULL) {
					fprintf(stderr, "Error: no network device configured\n");
					return 1;
				}
				if (br->iprange_start || br->iprange_end) {
					fprintf(stderr, "Error: cannot configure the IP range twice for the same interface\n");
					return 1;
				}
				
				char *firstip = argv[i] + 10;
				char *secondip = firstip;
				while (*secondip != '\0') {
					if (*secondip == ',')
						break;
					secondip++;
				}
				if (*secondip == '\0') {
					fprintf(stderr, "Error: invalid IP range\n");
					return 1;
				}
				*secondip = '\0';
				secondip++;
				
				if (atoip(firstip, &br->iprange_start) || atoip(secondip, &br->iprange_end) ||
				    br->iprange_start >= br->iprange_end) {
					fprintf(stderr, "Error: invalid IP range\n");
					return 1;
				}
				if (in_netrange(br->iprange_start, br->ip, br->mask) || in_netrange(br->iprange_end, br->ip, br->mask)) {
					fprintf(stderr, "Error: IP range addresses not in network range\n");
					return 1;
				}
			}
			else {
				fprintf(stderr, "Error: networking features are disabled in Firejail configuration file\n");
				exit(1);
			}
		}

		else if (strncmp(argv[i], "--mac=", 6) == 0) {
			if (checkcfg(CFG_NETWORK)) {
				Bridge *br = last_bridge_configured();
				if (br == NULL) {
					fprintf(stderr, "Error: no network device configured\n");
					exit(1);
				}
				if (mac_not_zero(br->macsandbox)) {
					fprintf(stderr, "Error: cannot configure the MAC address twice for the same interface\n");
					exit(1);
				}
	
				if (atomac(argv[i] + 6, br->macsandbox)) {
					fprintf(stderr, "Error: invalid MAC address\n");
					exit(1);
				}
			}
			else {
				fprintf(stderr, "Error: networking features are disabled in Firejail configuration file\n");
				exit(1);
			}
		}

		else if (strncmp(argv[i], "--mtu=", 6) == 0) {
			if (checkcfg(CFG_NETWORK)) {
				Bridge *br = last_bridge_configured();
				if (br == NULL) {
					fprintf(stderr, "Error: no network device configured\n");
					exit(1);
				}
	
				if (sscanf(argv[i] + 6, "%d", &br->mtu) != 1 || br->mtu < 576 || br->mtu > 9198) {
					fprintf(stderr, "Error: invalid mtu value\n");
					exit(1);
				}
			}
			else {
				fprintf(stderr, "Error: networking features are disabled in Firejail configuration file\n");
				exit(1);
			}
		}

		else if (strncmp(argv[i], "--ip=", 5) == 0) {
			if (checkcfg(CFG_NETWORK)) {
				Bridge *br = last_bridge_configured();
				if (br == NULL) {
					fprintf(stderr, "Error: no network device configured\n");
					exit(1);
				}
				if (br->arg_ip_none || br->ipsandbox) {
					fprintf(stderr, "Error: cannot configure the IP address twice for the same interface\n");
					exit(1);
				}
	
				if (strcmp(argv[i] + 5, "none") == 0)
					br->arg_ip_none = 1;
				else {
					if (atoip(argv[i] + 5, &br->ipsandbox)) {
						fprintf(stderr, "Error: invalid IP address\n");
						exit(1);
					}
				}
			}
			else {
				fprintf(stderr, "Error: networking features are disabled in Firejail configuration file\n");
				exit(1);
			}
		}

		else if (strncmp(argv[i], "--ip6=", 6) == 0) {
			if (checkcfg(CFG_NETWORK)) {
				Bridge *br = last_bridge_configured();
				if (br == NULL) {
					fprintf(stderr, "Error: no network device configured\n");
					exit(1);
				}
				if (br->arg_ip_none || br->ip6sandbox) {
					fprintf(stderr, "Error: cannot configure the IP address twice for the same interface\n");
					exit(1);
				}
	
				br->ip6sandbox = argv[i] + 6;
			}
			else {
				fprintf(stderr, "Error: networking features are disabled in Firejail configuration file\n");
				exit(1);
			}
		}


		else if (strncmp(argv[i], "--defaultgw=", 12) == 0) {
			if (checkcfg(CFG_NETWORK)) {
				if (atoip(argv[i] + 12, &cfg.defaultgw)) {
					fprintf(stderr, "Error: invalid IP address\n");
					exit(1);
				}
			}
			else {
				fprintf(stderr, "Error: networking features are disabled in Firejail configuration file\n");
				exit(1);
			}
		}
#endif		
		else if (strncmp(argv[i], "--dns=", 6) == 0) {
			uint32_t dns;
			if (atoip(argv[i] + 6, &dns)) {
				fprintf(stderr, "Error: invalid DNS server IP address\n");
				return 1;
			}
			
			if (cfg.dns1 == 0)
				cfg.dns1 = dns;
			else if (cfg.dns2 == 0)
				cfg.dns2 = dns;
			else if (cfg.dns3 == 0)
				cfg.dns3 = dns;
			else {
				fprintf(stderr, "Error: up to 3 DNS servers can be specified\n");
				return 1;
			}
		}

#ifdef HAVE_NETWORK
		else if (strcmp(argv[i], "--netfilter") == 0) {
#ifdef HAVE_NETWORK_RESTRICTED
			if (getuid() != 0) {
				fprintf(stderr, "Error: --netfilter is only allowed for root\n");
				exit(1);
			}
#endif
			if (checkcfg(CFG_RESTRICTED_NETWORK) && getuid() != 0) {
				fprintf(stderr, "Error: --netfilter is only allowed for root\n");
				exit(1);
			}
			if (checkcfg(CFG_NETWORK)) {
				arg_netfilter = 1;
			}
			else {
				fprintf(stderr, "Error: networking features are disabled in Firejail configuration file\n");
				exit(1);
			}
		}

		else if (strncmp(argv[i], "--netfilter=", 12) == 0) {
#ifdef HAVE_NETWORK_RESTRICTED
			if (getuid() != 0) {
				fprintf(stderr, "Error: --netfilter is only allowed for root\n");
				exit(1);
			}
#endif
			if (checkcfg(CFG_RESTRICTED_NETWORK) && getuid() != 0) {
				fprintf(stderr, "Error: --netfilter is only allowed for root\n");
				exit(1);
			}
			if (checkcfg(CFG_NETWORK)) {
				arg_netfilter = 1;
				arg_netfilter_file = argv[i] + 12;
				check_netfilter_file(arg_netfilter_file);
			}
			else {
				fprintf(stderr, "Error: networking features are disabled in Firejail configuration file\n");
				exit(1);
			}
		}

		else if (strncmp(argv[i], "--netfilter6=", 13) == 0) {
			if (checkcfg(CFG_NETWORK)) {
				arg_netfilter6 = 1;
				arg_netfilter6_file = argv[i] + 13;
				check_netfilter_file(arg_netfilter6_file);
			}
			else {
				fprintf(stderr, "Error: networking features are disabled in Firejail configuration file\n");
				exit(1);
			}
		}
#endif
		else if (strcmp(argv[i], "--audit") == 0) {
			if (asprintf(&arg_audit_prog, "%s/firejail/faudit", LIBDIR) == -1)
				errExit("asprintf");
			arg_audit = 1;
		}
		else if (strncmp(argv[i], "--audit=", 8) == 0) {
			if (strlen(argv[i] + 8) == 0) {
				fprintf(stderr, "Error: invalid audit program\n");
				exit(1);
			}
			arg_audit_prog = strdup(argv[i] + 8);
			if (!arg_audit_prog)
				errExit("strdup");
			arg_audit = 1;
		}
		else if (strcmp(argv[i], "--appimage") == 0)
			arg_appimage = 1;
		else if (strcmp(argv[i], "--csh") == 0) {
			if (arg_shell_none) {
			
				fprintf(stderr, "Error: --shell=none was already specified.\n");
				return 1;
			}
			if (cfg.shell) {
				fprintf(stderr, "Error: only one default user shell can be specified\n");
				return 1;
			}
			cfg.shell = "/bin/csh";
		}
		else if (strcmp(argv[i], "--zsh") == 0) {
			if (arg_shell_none) {
				fprintf(stderr, "Error: --shell=none was already specified.\n");
				return 1;
			}
			if (cfg.shell) {
				fprintf(stderr, "Error: only one default user shell can be specified\n");
				return 1;
			}
			cfg.shell = "/bin/zsh";
		}
		else if (strcmp(argv[i], "--shell=none") == 0) {
			arg_shell_none = 1;
			if (cfg.shell) {
				fprintf(stderr, "Error: a shell was already specified\n");
				return 1;
			}
		}
		else if (strncmp(argv[i], "--shell=", 8) == 0) {
			if (arg_shell_none) {
				fprintf(stderr, "Error: --shell=none was already specified.\n");
				return 1;
			}
			invalid_filename(argv[i] + 8);
			
			if (cfg.shell) {
				fprintf(stderr, "Error: only one user shell can be specified\n");
				return 1;
			}
			cfg.shell = argv[i] + 8;

			if (is_dir(cfg.shell) || strstr(cfg.shell, "..")) {
				fprintf(stderr, "Error: invalid shell\n");
				exit(1);
			}

			if(cfg.chrootdir) {
				char *shellpath;
				if (asprintf(&shellpath, "%s%s", cfg.chrootdir, cfg.shell) == -1)
					errExit("asprintf");
				if (access(shellpath, R_OK)) {
					fprintf(stderr, "Error: cannot access shell file in chroot\n");
					exit(1);
				}
				free(shellpath);
			} else if (access(cfg.shell, R_OK)) {
				fprintf(stderr, "Error: cannot access shell file\n");
				exit(1);
			}
		}
		else if (strcmp(argv[i], "-c") == 0) {
			arg_command = 1;
			if (i == (argc -  1)) {
				fprintf(stderr, "Error: option -c requires an argument\n");
				return 1;
			}
		}
		
		else if (strcmp(argv[i], "--x11=none") == 0) {
			arg_x11_block = 1;
		}
#ifdef HAVE_X11
		else if (strcmp(argv[i], "--x11=xorg") == 0) {
			if (checkcfg(CFG_X11))
				arg_x11_xorg = 1;
			else {
				fprintf(stderr, "Error: --x11 feature is disabled in Firejail configuration file\n");
				exit(1);
			}
		}
#endif
		else if (strncmp(argv[i], "--join-or-start=", 16) == 0) {

			cfg.name = argv[i] + 16;
			if (strlen(cfg.name) == 0) {
				fprintf(stderr, "Error: please provide a name for sandbox\n");
				return 1;
			}
		}
		else if (strcmp(argv[i], "--") == 0) {
			arg_doubledash = 1;
			i++;
			if (i  >= argc) {
				fprintf(stderr, "Error: program name not found\n");
				exit(1);
			}
			extract_command_name(i, argv);
			prog_index = i;
			break;
		}
		else {
			if (*argv[i] == '-') {
				fprintf(stderr, "Error: invalid %s command line option\n", argv[i]);
				return 1;
			}
			
			if (arg_appimage) {
				cfg.command_name = strdup(argv[i]);
				if (!cfg.command_name)
					errExit("strdup");
			}
			else
				extract_command_name(i, argv);
			prog_index = i;
			break;
		}
	}
	
	if (prog_index == -1 && arg_shell_none) {
		fprintf(stderr, "shell=none configured, but no program specified\n");
		exit(1);
	}

	if (arg_trace && arg_tracelog) {
		if (!arg_quiet || arg_debug)
			fprintf(stderr, "Warning: --trace and --tracelog are mutually exclusive; --tracelog disabled\n");
	}
	
	if (getenv("FIREJAIL_X11"))
		mask_x11_abstract_socket = 1;
	
	if (arg_noroot) {
		if (arg_overlay) {
			fprintf(stderr, "Error: --overlay and --noroot are mutually exclusive.\n");
			exit(1);
		}
		else if (cfg.chrootdir) {
			fprintf(stderr, "Error: --chroot and --noroot are mutually exclusive.\n");
			exit(1);
		}
	}

	logargs(argc, argv);
	if (fullargc) {
		char *msg;
		if (asprintf(&msg, "user %s entering restricted shell", cfg.username) == -1)
			errExit("asprintf");
		logmsg(msg);
		free(msg);
	}

	if (!arg_shell_none && !cfg.shell) {
		cfg.shell = guess_shell();
		if (!cfg.shell) {
			fprintf(stderr, "Error: unable to guess your shell, please set explicitly by using --shell option.\n");
			exit(1);
		}
		if (arg_debug)
			printf("Autoselecting %s as shell\n", cfg.shell);
	}

	if (prog_index == -1 && cfg.shell) {
		cfg.command_line = cfg.shell;
		cfg.window_title = cfg.shell;
		cfg.command_name = cfg.shell;
	}
	else if (arg_appimage) {
		if (arg_debug)
			printf("Configuring appimage environment\n");
		appimage_set(cfg.command_name);
		cfg.window_title = "appimage";
	}
	else {
		build_cmdline(&cfg.command_line, &cfg.window_title, argc, argv, prog_index);
	}
/*	else {
		fprintf(stderr, "Error: command must be specified when --shell=none used.\n");
		exit(1);
	}*/
	
	assert(cfg.command_name);
	if (arg_debug)
		printf("Command name #%s#\n", cfg.command_name);
		
				
	if (!arg_noprofile) {
		if (!custom_profile) {
			char *usercfgdir;
			if (asprintf(&usercfgdir, "%s/.config/firejail", cfg.homedir) == -1)
				errExit("asprintf");
			int rv = profile_find(cfg.command_name, usercfgdir);
			free(usercfgdir);
			custom_profile = rv;
		}
		if (!custom_profile) {
			int rv;
			if (custom_profile_dir)
				rv = profile_find(cfg.command_name, custom_profile_dir);
			else
				rv = profile_find(cfg.command_name, SYSCONFDIR);
			custom_profile = rv;
		}
	}

	if (!custom_profile && !arg_noprofile) {
		if (cfg.chrootdir) {
			if (!arg_quiet || arg_debug)
				fprintf(stderr, "Warning: default profile disabled by --chroot option\n");
		}
		else if (arg_overlay) {
			if (!arg_quiet || arg_debug)
				fprintf(stderr, "Warning: default profile disabled by --overlay option\n");
		}
		else {
			char *profile_name = DEFAULT_USER_PROFILE;
			if (getuid() == 0)
				profile_name = DEFAULT_ROOT_PROFILE;
			if (arg_debug)
				printf("Attempting to find %s.profile...\n", profile_name);
	
			char *usercfgdir;
			if (asprintf(&usercfgdir, "%s/.config/firejail", cfg.homedir) == -1)
				errExit("asprintf");
			custom_profile = profile_find(profile_name, usercfgdir);
			free(usercfgdir);
	
			if (!custom_profile) {
				if (custom_profile_dir)
					custom_profile = profile_find(profile_name, custom_profile_dir);
				else
					custom_profile = profile_find(profile_name, SYSCONFDIR);
			}
			if (!custom_profile) {
				fprintf(stderr, "Error: no default.profile installed\n");
				exit(1);
			}
			
			if (custom_profile && !arg_quiet)
				printf("\n** Note: you can use --noprofile to disable %s.profile **\n\n", profile_name);
		}
	}

	if (arg_x11_block)
		x11_block();

	net_check_cfg();
	
	if (any_bridge_configured()) {
		EUID_ROOT();
		lockfd = open(RUN_NETWORK_LOCK_FILE, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
		if (lockfd != -1) {
			int rv = fchown(lockfd, 0, 0);
			(void) rv;
			flock(lockfd, LOCK_EX);
		}
		
		check_network(&cfg.bridge0);
		check_network(&cfg.bridge1);
		check_network(&cfg.bridge2);
		check_network(&cfg.bridge3);
			
		network_set_run_file(sandbox_pid);
		EUID_USER();
	}

 	if (pipe(parent_to_child_fds) < 0)
 		errExit("pipe");
 	if (pipe(child_to_parent_fds) < 0)
		errExit("pipe");

	if (arg_noroot && arg_overlay) {
		if (!arg_quiet || arg_debug)
			fprintf(stderr, "Warning: --overlay and --noroot are mutually exclusive, noroot disabled\n");
		arg_noroot = 0;
	}
	else if (arg_noroot && cfg.chrootdir) {
		if (!arg_quiet || arg_debug)
			fprintf(stderr, "Warning: --chroot and --noroot are mutually exclusive, noroot disabled\n");
		arg_noroot = 0;
	}


	EUID_ROOT();
	if (cfg.name)
		set_name_file(sandbox_pid);
	int display = x11_display();
	if (display > 0)
		set_x11_file(sandbox_pid, display);
	EUID_USER();
	
	int flags = CLONE_NEWNS | CLONE_NEWPID | CLONE_NEWUTS | SIGCHLD;
	
	if (getuid() == 0 || arg_ipc) {
		flags |= CLONE_NEWIPC;
		if (arg_debug)
			printf("Enabling IPC namespace\n");
	}
	
	if (any_bridge_configured() || any_interface_configured() || arg_nonetwork) {
		flags |= CLONE_NEWNET;
	}
	else if (arg_debug)
		printf("Using the local network stack\n");

	EUID_ROOT();
	child = clone(sandbox,
		child_stack + STACK_SIZE,
		flags,
		NULL);
	if (child == -1)
		errExit("clone");
	EUID_USER();

	if (!arg_command && !arg_quiet) {
		printf("Parent pid %u, child pid %u\n", sandbox_pid, child);
		if (getuid() == 0) // only for root
			printf("The new log directory is /proc/%d/root/var/log\n", child);
	}
	
	if (!arg_nonetwork) {
		EUID_ROOT();	
		pid_t net_child = fork();
		if (net_child < 0)
			errExit("fork");
		if (net_child == 0) {
			if (setreuid(0, 0))
				errExit("setreuid");
			if (setregid(0, 0))
				errExit("setregid");
			network_main(child);
			if (arg_debug)
				printf("Host network configured\n");			
			_exit(0);			
		}

		waitpid(net_child, NULL, 0);
		EUID_USER();
	}

 	close(parent_to_child_fds[0]);
 	close(child_to_parent_fds[1]);
 
 	notify_other(parent_to_child_fds[1]);
 
 	wait_for_other(child_to_parent_fds[0]);
 	close(child_to_parent_fds[0]);

 	if (arg_noroot) {
	 	char *map_path;
	 	if (asprintf(&map_path, "/proc/%d/uid_map", child) == -1)
	 		errExit("asprintf");

	 	char *map;
	 	uid_t uid = getuid();
	 	if (asprintf(&map, "%d %d 1", uid, uid) == -1)
	 		errExit("asprintf");
 		EUID_ROOT();
	 	update_map(map, map_path);
	 	EUID_USER();
	 	free(map);
	 	free(map_path);
	 
		if (asprintf(&map_path, "/proc/%d/gid_map", child) == -1)
			errExit("asprintf");
	 	char gidmap[1024];
	 	char *ptr = gidmap;
	 	*ptr = '\0';

	 	gid_t gid = getgid();
	 	sprintf(ptr, "%d %d 1\n", gid, gid);
	 	ptr += strlen(ptr);
	 	
	 	gid_t g = get_group_id("tty");
	 	if (g) {
	 		sprintf(ptr, "%d %d 1\n", g, g);
	 		ptr += strlen(ptr);
	 	}
	 	
	 	g = get_group_id("audio");
	 	if (g) {
	 		sprintf(ptr, "%d %d 1\n", g, g);
	 		ptr += strlen(ptr);
	 	}
	 	
	 	g = get_group_id("video");
	 	if (g) {
	 		sprintf(ptr, "%d %d 1\n", g, g);
	 		ptr += strlen(ptr);
	 	}
	 	
	 	g = get_group_id("games");
	 	if (g) {
	 		sprintf(ptr, "%d %d 1\n", g, g);
	 	}
	 	
 		EUID_ROOT();
	 	update_map(gidmap, map_path);
	 	EUID_USER();
	 	free(map_path);
 	}
 	
 	notify_other(parent_to_child_fds[1]);
 	close(parent_to_child_fds[1]);
 
 	EUID_ROOT();
	if (lockfd != -1) {
		flock(lockfd, LOCK_UN);
		close(lockfd);
	}

	

	signal (SIGINT, my_handler);
	signal (SIGTERM, my_handler);

	
	EUID_USER();
	int status = 0;
	waitpid(child, &status, 0);

#ifdef HAVE_SECCOMP
	if (cfg.seccomp_list_errno) {
		for (i = 0; i < highest_errno; i++)
			free(cfg.seccomp_list_errno[i]);
		free(cfg.seccomp_list_errno);
	}
#endif
	if (cfg.profile) {
		ProfileEntry *prf = cfg.profile;
		while (prf != NULL) {
			ProfileEntry *next = prf->next;
			free(prf->data);
			free(prf->link);
			free(prf);
			prf = next;
		}
	}

	if (WIFEXITED(status)){
		myexit(WEXITSTATUS(status));
	} else if (WIFSIGNALED(status)) {
		myexit(WTERMSIG(status));
	} else {
		myexit(0);
	}

	return 0;
}
