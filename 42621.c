static int buildid_dir_command_config(const char *var, const char *value,
				      void *data)
{
	struct buildid_dir_config *c = data;
	const char *v;

	/* same dir for all commands */
	if (!prefixcmp(var, "buildid.") && !strcmp(var + 8, "dir")) {
		v = perf_config_dirname(var, value);
		if (!v)
			return -1;
		strncpy(c->dir, v, MAXPATHLEN-1);
		c->dir[MAXPATHLEN-1] = '\0';
	}
	return 0;
}
