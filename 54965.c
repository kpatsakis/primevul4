process_readdir(u_int32_t id)
{
	DIR *dirp;
	struct dirent *dp;
	char *path;
	int r, handle;

	if ((r = get_handle(iqueue, &handle)) != 0)
		fatal("%s: buffer error: %s", __func__, ssh_err(r));

	debug("request %u: readdir \"%s\" (handle %d)", id,
	    handle_to_name(handle), handle);
	dirp = handle_to_dir(handle);
	path = handle_to_name(handle);
	if (dirp == NULL || path == NULL) {
		send_status(id, SSH2_FX_FAILURE);
	} else {
		struct stat st;
		char pathname[PATH_MAX];
		Stat *stats;
		int nstats = 10, count = 0, i;

		stats = xcalloc(nstats, sizeof(Stat));
		while ((dp = readdir(dirp)) != NULL) {
			if (count >= nstats) {
				nstats *= 2;
				stats = xreallocarray(stats, nstats, sizeof(Stat));
			}
/* XXX OVERFLOW ? */
			snprintf(pathname, sizeof pathname, "%s%s%s", path,
			    strcmp(path, "/") ? "/" : "", dp->d_name);
			if (lstat(pathname, &st) < 0)
				continue;
			stat_to_attrib(&st, &(stats[count].attrib));
			stats[count].name = xstrdup(dp->d_name);
			stats[count].long_name = ls_file(dp->d_name, &st, 0, 0);
			count++;
			/* send up to 100 entries in one message */
			/* XXX check packet size instead */
			if (count == 100)
				break;
		}
		if (count > 0) {
			send_names(id, count, stats);
			for (i = 0; i < count; i++) {
				free(stats[i].name);
				free(stats[i].long_name);
			}
		} else {
			send_status(id, SSH2_FX_EOF);
		}
		free(stats);
	}
}
