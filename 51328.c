void drop_privileges(char *username) {
	struct passwd *user = (struct passwd *) getpwnam(username);
	if (user == NULL) {
		fprintf(stderr, _("Failed dropping privileges. The user %s is not a valid username on local system.\n"), username);
		exit(1);
	}
	if (getuid() == 0) {
		/* process is running as root, drop privileges */
		if (setgid(user->pw_gid) != 0) {
			fprintf(stderr, _("setgid: Error dropping group privileges\n"));
			exit(1);
		}
		if (setuid(user->pw_uid) != 0) {
			fprintf(stderr, _("setuid: Error dropping user privileges\n"));
			exit(1);
		}
		/* Verify if the privileges were developed. */
		if (setuid(0) != -1) {
			fprintf(stderr, _("Failed to drop privileges\n"));
			exit(1);
		}
	}
}
