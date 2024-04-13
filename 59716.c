static void set_name_file(pid_t pid) {
	char *fname;
	if (asprintf(&fname, "%s/%d", RUN_FIREJAIL_NAME_DIR, pid) == -1)
		errExit("asprintf");

	FILE *fp = fopen(fname, "w");
	if (!fp) {
		fprintf(stderr, "Error: cannot create %s\n", fname);
		exit(1);
	}
	fprintf(fp, "%s\n", cfg.name);

	SET_PERMS_STREAM(fp, 0, 0, 0644);
	fclose(fp);
}
