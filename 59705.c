static void read_bandwidth_file(pid_t pid) {
	assert(ifbw == NULL);

	char *fname;
	if (asprintf(&fname, "%s/%d-bandwidth", RUN_FIREJAIL_BANDWIDTH_DIR, (int) pid) == -1)
		errExit("asprintf");

	FILE *fp = fopen(fname, "r");
	if (fp) {
		char buf[1024];
		while (fgets(buf, 1024,fp)) {
			char *ptr = strchr(buf, '\n');
			if (ptr)
				*ptr = '\0';
			if (strlen(buf) == 0)
				continue;
			
			IFBW *ifbw_new = malloc(sizeof(IFBW));
			if (!ifbw_new)
				errExit("malloc");
			memset(ifbw_new, 0, sizeof(IFBW));
			ifbw_new->txt = strdup(buf);
			if (!ifbw_new->txt)
				errExit("strdup");
			
			ifbw_add(ifbw_new);
		}	
		
		fclose(fp);		
	}
}
