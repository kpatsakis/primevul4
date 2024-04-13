void network_set_run_file(pid_t pid) {
	char *fname;
	if (asprintf(&fname, "%s/%d-netmap", RUN_FIREJAIL_NETWORK_DIR, (int) pid) == -1)
		errExit("asprintf");
	
	FILE *fp = fopen(fname, "w");
	if (fp) {
		if (cfg.bridge0.configured)
			fprintf(fp, "%s:%s\n", cfg.bridge0.dev, cfg.bridge0.devsandbox);
		if (cfg.bridge1.configured)
			fprintf(fp, "%s:%s\n", cfg.bridge1.dev, cfg.bridge1.devsandbox);
		if (cfg.bridge2.configured)
			fprintf(fp, "%s:%s\n", cfg.bridge2.dev, cfg.bridge2.devsandbox);
		if (cfg.bridge3.configured)
			fprintf(fp, "%s:%s\n", cfg.bridge3.dev, cfg.bridge3.devsandbox);

		SET_PERMS_STREAM(fp, 0, 0, 0644);
		fclose(fp);
	}
	else {
		fprintf(stderr, "Error: cannot create network map file\n");
		exit(1);
	}
	
	free(fname);
}
