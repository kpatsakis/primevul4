static void run_cmd_and_exit(int i, int argc, char **argv) {
	EUID_ASSERT();
	
	if (strcmp(argv[i], "--help") == 0 ||
	    strcmp(argv[i], "-?") == 0) {
		usage();
		exit(0);
	}
	else if (strcmp(argv[i], "--version") == 0) {
		printf("firejail version %s\n", VERSION);
		printf("\n");
		print_compiletime_support();
		printf("\n");
		exit(0);
	}
#ifdef HAVE_OVERLAYFS
	else if (strcmp(argv[i], "--overlay-clean") == 0) {
		if (checkcfg(CFG_OVERLAYFS)) {
			char *path;
			if (asprintf(&path, "%s/.firejail", cfg.homedir) == -1)
				errExit("asprintf");
			EUID_ROOT();
			if (setreuid(0, 0) < 0)
				errExit("setreuid");
			if (setregid(0, 0) < 0)
				errExit("setregid");
			errno = 0;
			int rv = remove_directory(path);
			if (rv) {
				fprintf(stderr, "Error: cannot removed overlays stored in ~/.firejail directory, errno %d\n", errno);
				exit(1);
			}
		}
		else {
			fprintf(stderr, "Error: overlayfs feature is disabled in Firejail configuration file\n");
			exit(1);
		}
		exit(0);
	}
#endif
#ifdef HAVE_X11
	else if (strcmp(argv[i], "--x11") == 0) {
		if (checkcfg(CFG_X11)) {
			x11_start(argc, argv);
			exit(0);
		}
		else {
			fprintf(stderr, "Error: --x11 feature is disabled in Firejail configuration file\n");
			exit(1);
		}
	}
	else if (strcmp(argv[i], "--x11=xpra") == 0) {
		if (checkcfg(CFG_X11)) {
			x11_start_xpra(argc, argv);
			exit(0);
		}
		else {
			fprintf(stderr, "Error: --x11 feature is disabled in Firejail configuration file\n");
			exit(1);
		}
	}
	else if (strcmp(argv[i], "--x11=xephyr") == 0) {
		if (checkcfg(CFG_X11)) {
			x11_start_xephyr(argc, argv);
			exit(0);
		}
		else {
			fprintf(stderr, "Error: --x11 feature is disabled in Firejail configuration file\n");
			exit(1);
		}
	}
#endif
#ifdef HAVE_NETWORK	
	else if (strncmp(argv[i], "--bandwidth=", 12) == 0) {
		if (checkcfg(CFG_NETWORK)) {
			logargs(argc, argv);
			
			if ((i + 1) == argc) {
				fprintf(stderr, "Error: command expected after --bandwidth option\n");
				exit(1);
			}
			char *cmd = argv[i + 1];
			if (strcmp(cmd, "status") && strcmp(cmd, "clear") && strcmp(cmd, "set")) {
				fprintf(stderr, "Error: invalid --bandwidth command.\nValid commands: set, clear, status.\n");
				exit(1);
			}
	
			char *dev = NULL;
			int down = 0;
			int up = 0;
			if (strcmp(cmd, "set") == 0 || strcmp(cmd, "clear") == 0) {
				if ((i + 2) == argc) {
					fprintf(stderr, "Error: network name expected after --bandwidth %s option\n", cmd);
					exit(1);
				}
				dev = argv[i + 2];
	
				if (if_nametoindex(dev) == 0) {
					fprintf(stderr, "Error: network device %s not found\n", dev);
					exit(1);
				}
	
				if (strcmp(cmd, "set") == 0) {
					if ((i + 4) >= argc) {
						fprintf(stderr, "Error: invalid --bandwidth set command\n");
						exit(1);
					}
					
					down = atoi(argv[i + 3]);
					if (down < 0) {
						fprintf(stderr, "Error: invalid download speed\n");
						exit(1);
					}
					up = atoi(argv[i + 4]);
					if (up < 0) {
						fprintf(stderr, "Error: invalid upload speed\n");
						exit(1);
					}
				}
			}	
			
			pid_t pid;
			if (read_pid(argv[i] + 12, &pid) == 0)
				bandwidth_pid(pid, cmd, dev, down, up);
			else
				bandwidth_name(argv[i] + 12, cmd, dev, down, up);
		}
		else {
			fprintf(stderr, "Error: networking features are disabled in Firejail configuration file\n");
			exit(1);
		}
		exit(0);
	}
#endif
#ifdef HAVE_SECCOMP
	else if (strcmp(argv[i], "--debug-syscalls") == 0) {
		if (checkcfg(CFG_SECCOMP)) {
			syscall_print();
			exit(0);
		}
		else {
			fprintf(stderr, "Error: seccomp feature is disabled in Firejail configuration file\n");
			exit(1);
		}
	}
	else if (strcmp(argv[i], "--debug-errnos") == 0) {
		if (checkcfg(CFG_SECCOMP)) {
			errno_print();
		}
		else {
			fprintf(stderr, "Error: seccomp feature is disabled in Firejail configuration file\n");
			exit(1);
		}
		exit(0);
	}
	else if (strncmp(argv[i], "--seccomp.print=", 16) == 0) {
		if (checkcfg(CFG_SECCOMP)) {
			pid_t pid;
			if (read_pid(argv[i] + 16, &pid) == 0)		
				seccomp_print_filter(pid);
			else
				seccomp_print_filter_name(argv[i] + 16);
		}
		else {
			fprintf(stderr, "Error: seccomp feature is disabled in Firejail configuration file\n");
			exit(1);
		}
		exit(0);
	}
	else if (strcmp(argv[i], "--debug-protocols") == 0) {
		protocol_list();
		exit(0);
	}
	else if (strncmp(argv[i], "--protocol.print=", 17) == 0) {
		if (checkcfg(CFG_SECCOMP)) {
			pid_t pid;
			if (read_pid(argv[i] + 17, &pid) == 0)		
				protocol_print_filter(pid);
			else
				protocol_print_filter_name(argv[i] + 17);
		}
		else {
			fprintf(stderr, "Error: seccomp feature is disabled in Firejail configuration file\n");
			exit(1);
		}
		exit(0);
	}
#endif
	else if (strncmp(argv[i], "--cpu.print=", 12) == 0) {
		pid_t pid;
		if (read_pid(argv[i] + 12, &pid) == 0)		
			cpu_print_filter(pid);
		else
			cpu_print_filter_name(argv[i] + 12);
		exit(0);
	}
	else if (strncmp(argv[i], "--caps.print=", 13) == 0) {
		pid_t pid;
		if (read_pid(argv[i] + 13, &pid) == 0)		
			caps_print_filter(pid);
		else
			caps_print_filter_name(argv[i] + 13);
		exit(0);
	}
	else if (strncmp(argv[i], "--fs.print=", 11) == 0) {
		pid_t pid;
		if (read_pid(argv[i] + 11, &pid) == 0)		
			fs_logger_print_log(pid);
		else
			fs_logger_print_log_name(argv[i] + 11);
		exit(0);
	}
	else if (strncmp(argv[i], "--dns.print=", 12) == 0) {
		pid_t pid;
		if (read_pid(argv[i] + 12, &pid) == 0)		
			net_dns_print(pid);
		else
			net_dns_print_name(argv[i] + 12);
		exit(0);
	}
	else if (strcmp(argv[i], "--debug-caps") == 0) {
		caps_print();
		exit(0);
	}
	else if (strcmp(argv[i], "--list") == 0) {
		list();
		exit(0);
	}
	else if (strcmp(argv[i], "--tree") == 0) {
		tree();
		exit(0);
	}
	else if (strcmp(argv[i], "--top") == 0) {
		top();
		exit(0);
	}
#ifdef HAVE_NETWORK	
	else if (strcmp(argv[i], "--netstats") == 0) {
		if (checkcfg(CFG_NETWORK)) {
			netstats();
		}
		else {
			fprintf(stderr, "Error: networking features are disabled in Firejail configuration file\n");
			exit(1);
		}
		exit(0);
	}
#endif	
#ifdef HAVE_FILE_TRANSFER
	else if (strncmp(argv[i], "--get=", 6) == 0) {
		if (checkcfg(CFG_FILE_TRANSFER)) {
			logargs(argc, argv);
			
			if ((i + 2) != argc) {
				fprintf(stderr, "Error: invalid --get option, path expected\n");
				exit(1);
			}
			char *path = argv[i + 1];
			 invalid_filename(path);
			 if (strstr(path, "..")) {
			 	fprintf(stderr, "Error: invalid file name %s\n", path);
			 	exit(1);
			 }
			 
			pid_t pid;
			if (read_pid(argv[i] + 6, &pid) == 0)		
				sandboxfs(SANDBOX_FS_GET, pid, path, NULL);
			else
				sandboxfs_name(SANDBOX_FS_GET, argv[i] + 6, path, NULL);
			exit(0);
		}
		else {
			fprintf(stderr, "Error: --get feature is disabled in Firejail configuration file\n");
			exit(1);
		}
	}
	else if (strncmp(argv[i], "--put=", 6) == 0) {
		if (checkcfg(CFG_FILE_TRANSFER)) {
			logargs(argc, argv);
			
			if ((i + 3) != argc) {
				fprintf(stderr, "Error: invalid --put option, 2 paths expected\n");
				exit(1);
			}
			char *path1 = argv[i + 1];
			 invalid_filename(path1);
			 if (strstr(path1, "..")) {
			 	fprintf(stderr, "Error: invalid file name %s\n", path1);
			 	exit(1);
			 }
			char *path2 = argv[i + 2];
			 invalid_filename(path2);
			 if (strstr(path2, "..")) {
			 	fprintf(stderr, "Error: invalid file name %s\n", path2);
			 	exit(1);
			 }
			 
			pid_t pid;
			if (read_pid(argv[i] + 6, &pid) == 0)		
				sandboxfs(SANDBOX_FS_PUT, pid, path1, path2);
			else
				sandboxfs_name(SANDBOX_FS_PUT, argv[i] + 6, path1, path2);
			exit(0);
		}
		else {
			fprintf(stderr, "Error: --get feature is disabled in Firejail configuration file\n");
			exit(1);
		}
	}
	else if (strncmp(argv[i], "--ls=", 5) == 0) {
		if (checkcfg(CFG_FILE_TRANSFER)) {
			logargs(argc, argv);
			
			if ((i + 2) != argc) {
				fprintf(stderr, "Error: invalid --ls option, path expected\n");
				exit(1);
			}
			char *path = argv[i + 1];
			 invalid_filename(path);
			 if (strstr(path, "..")) {
			 	fprintf(stderr, "Error: invalid file name %s\n", path);
			 	exit(1);
			 }
			 
			pid_t pid;
			if (read_pid(argv[i] + 5, &pid) == 0)		
				sandboxfs(SANDBOX_FS_LS, pid, path, NULL);
			else
				sandboxfs_name(SANDBOX_FS_LS, argv[i] + 5, path, NULL);
			exit(0);
		}
		else {
			fprintf(stderr, "Error: --ls feature is disabled in Firejail configuration file\n");
			exit(1);
		}
	}
#endif
	else if (strncmp(argv[i], "--join=", 7) == 0) {
		logargs(argc, argv);

		if (arg_shell_none) {
			if (argc <= (i+1)) {
				fprintf(stderr, "Error: --shell=none set, but no command specified\n");
				exit(1);
			}
			cfg.original_program_index = i + 1;
		}

		if (!cfg.shell && !arg_shell_none)
			cfg.shell = guess_shell();

		pid_t pid;
		if (read_pid(argv[i] + 7, &pid) == 0)		
			join(pid, argc, argv, i + 1);
		else
			join_name(argv[i] + 7, argc, argv, i + 1);
		exit(0);

	}
	else if (strncmp(argv[i], "--join-or-start=", 16) == 0) {
		logargs(argc, argv);

		if (arg_shell_none) {
			if (argc <= (i+1)) {
				fprintf(stderr, "Error: --shell=none set, but no command specified\n");
				exit(1);
			}
			cfg.original_program_index = i + 1;
		}

		pid_t pid;
		if (!name2pid(argv[i] + 16, &pid)) {
			if (!cfg.shell && !arg_shell_none)
				cfg.shell = guess_shell();

			join(pid, argc, argv, i + 1);
			exit(0);
		}
	}
#ifdef HAVE_NETWORK	
	else if (strncmp(argv[i], "--join-network=", 15) == 0) {
		if (checkcfg(CFG_NETWORK)) {
			logargs(argc, argv);
			arg_join_network = 1;
			if (getuid() != 0) {
				fprintf(stderr, "Error: --join-network is only available to root user\n");
				exit(1);
			}
			
			if (!cfg.shell && !arg_shell_none)
				cfg.shell = guess_shell();

			pid_t pid;
			if (read_pid(argv[i] + 15, &pid) == 0)		
				join(pid, argc, argv, i + 1);
			else
				join_name(argv[i] + 15, argc, argv, i + 1);
		}
		else {
			fprintf(stderr, "Error: networking features are disabled in Firejail configuration file\n");
			exit(1);
		}

		exit(0);
	}
#endif
	else if (strncmp(argv[i], "--join-filesystem=", 18) == 0) {
		logargs(argc, argv);
		arg_join_filesystem = 1;
		if (getuid() != 0) {
			fprintf(stderr, "Error: --join-filesystem is only available to root user\n");
			exit(1);
		}
		
		if (!cfg.shell && !arg_shell_none)
			cfg.shell = guess_shell();

		pid_t pid;
		if (read_pid(argv[i] + 18, &pid) == 0)		
			join(pid, argc, argv, i + 1);
		else
			join_name(argv[i] + 18, argc, argv, i + 1);
		exit(0);
	}
	else if (strncmp(argv[i], "--shutdown=", 11) == 0) {
		logargs(argc, argv);
		
		pid_t pid;
		if (read_pid(argv[i] + 11, &pid) == 0)
			shut(pid);
		else
			shut_name(argv[i] + 11);
		exit(0);
	}

}
