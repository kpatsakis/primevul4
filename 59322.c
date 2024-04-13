int main(int argc, char *argv[])
{
	int n, fd;
	bool gotone = false;
	char *me;
	char *nicname = alloca(40);
	char *cnic = NULL; // created nic name in container is returned here.
	char *vethname = NULL;
	int pid;
	struct alloted_s *alloted = NULL;

	/* set a sane env, because we are setuid-root */
	if (clearenv() < 0) {
		fprintf(stderr, "Failed to clear environment");
		exit(1);
	}
	if (setenv("PATH", "/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin", 1) < 0) {
		fprintf(stderr, "Failed to set PATH, exiting\n");
		exit(1);
	}
	if ((me = get_username()) == NULL) {
		fprintf(stderr, "Failed to get username\n");
		exit(1);
	}

	if (argc < 6)
		usage(argv[0], true);
	if (argc >= 7)
		vethname = argv[6];

	lxcpath = argv[1];
	lxcname = argv[2];

	errno = 0;
	pid = (int) strtol(argv[3], NULL, 10);
	if (errno) {
		fprintf(stderr, "Could not read pid: %s\n", argv[1]);
		exit(1);
	}

	if (!create_db_dir(LXC_USERNIC_DB)) {
		fprintf(stderr, "Failed to create directory for db file\n");
		exit(1);
	}

	if ((fd = open_and_lock(LXC_USERNIC_DB)) < 0) {
		fprintf(stderr, "Failed to lock %s\n", LXC_USERNIC_DB);
		exit(1);
	}

	if (!may_access_netns(pid)) {
		fprintf(stderr, "User %s may not modify netns for pid %d\n",
			me, pid);
		exit(1);
	}

	n = get_alloted(me, argv[4], argv[5], &alloted);
	if (n > 0)
		gotone = get_nic_if_avail(fd, alloted, pid, argv[4], argv[5], n, &nicname, &cnic);

	close(fd);
	free_alloted(&alloted);
	if (!gotone) {
		fprintf(stderr, "Quota reached\n");
		exit(1);
	}

	if (rename_in_ns(pid, cnic, &vethname) < 0) {
		fprintf(stderr, "Failed to rename the link\n");
		exit(1);
	}

	fprintf(stdout, "%s:%s\n", vethname, nicname);
	exit(0);
}
