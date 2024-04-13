void set_buildid_dir(void)
{
	buildid_dir[0] = '\0';

	/* try config file */
	check_buildid_dir_config();

	/* default to $HOME/.debug */
	if (buildid_dir[0] == '\0') {
		char *v = getenv("HOME");
		if (v) {
			snprintf(buildid_dir, MAXPATHLEN-1, "%s/%s",
				 v, DEBUG_CACHE_DIR);
		} else {
			strncpy(buildid_dir, DEBUG_CACHE_DIR, MAXPATHLEN-1);
		}
		buildid_dir[MAXPATHLEN-1] = '\0';
	}
	/* for communicating with external commands */
	setenv("PERF_BUILDID_DIR", buildid_dir, 1);
}
