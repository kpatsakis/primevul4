static void set_x11_file(pid_t pid, int display) {
	char *fname;
	if (asprintf(&fname, "%s/%d", RUN_FIREJAIL_X11_DIR, pid) == -1)
		errExit("asprintf");

	FILE *fp = fopen(fname, "w");
	if (!fp) {
		fprintf(stderr, "Error: cannot create %s\n", fname);
		exit(1);
	}
	fprintf(fp, "%d\n", display);

	SET_PERMS_STREAM(fp, 0, 0, 0644);
	fclose(fp);
}
