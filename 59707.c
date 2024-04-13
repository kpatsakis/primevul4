void check_user_namespace(void) {
	EUID_ASSERT();
	if (getuid() == 0) {
		fprintf(stderr, "Error: --noroot option cannot be used when starting the sandbox as root.\n");
		exit(1);
	}
	
	struct stat s1;
	struct stat s2;
	struct stat s3;
	if (stat("/proc/self/ns/user", &s1) == 0 &&
	    stat("/proc/self/uid_map", &s2) == 0 &&
	    stat("/proc/self/gid_map", &s3) == 0)
		arg_noroot = 1;
	else {
		if (!arg_quiet || arg_debug)
			fprintf(stderr, "Warning: user namespaces not available in the current kernel.\n");
		arg_noroot = 0;
	}
}
