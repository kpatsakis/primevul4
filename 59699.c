static void bandwidth_create_run_file(pid_t pid) {
	char *fname;
	if (asprintf(&fname, "%s/%d-bandwidth", RUN_FIREJAIL_BANDWIDTH_DIR, (int) pid) == -1)
		errExit("asprintf");
	
	struct stat s;
	if (stat(fname, &s) == 0) {
		free(fname);
		return;
	}

	/* coverity[toctou] */
	FILE *fp = fopen(fname, "w");
	if (fp) {
		SET_PERMS_STREAM(fp, 0, 0, 0644);
		fclose(fp);
	}
	else {
		fprintf(stderr, "Error: cannot create bandwidth file\n");
		exit(1);
	}
	
	free(fname);
}
