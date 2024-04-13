GArray* do_cfile_dir(gchar* dir, GError** e) {
	DIR* dirh = opendir(dir);
	struct dirent* de;
	gchar* fname;
	GArray* retval = NULL;
	GArray* tmp;
	struct stat stbuf;

	if(!dir) {
		g_set_error(e, NBDS_ERR, NBDS_ERR_CFILE_DIR_UNKNOWN, "Invalid directory specified: %s", strerror(errno));
		return NULL;
	}
	errno=0;
	while((de = readdir(dirh))) {
		int saved_errno=errno;
		fname = g_build_filename(dir, de->d_name, NULL);
		switch(de->d_type) {
			case DT_UNKNOWN:
				/* Filesystem doesn't return type of
				 * file through readdir. Run stat() on
				 * the file instead */
				if(stat(fname, &stbuf)) {
					perror("stat");
					goto err_out;
				}
				if (!S_ISREG(stbuf.st_mode)) {
					goto next;
				}
			case DT_REG:
				/* Skip unless the name ends with '.conf' */
				if(strcmp((de->d_name + strlen(de->d_name) - 5), ".conf")) {
					goto next;
				}
				tmp = parse_cfile(fname, NULL, e);
				errno=saved_errno;
				if(*e) {
					goto err_out;
				}
				if(!retval)
					retval = g_array_new(FALSE, TRUE, sizeof(SERVER));
				retval = g_array_append_vals(retval, tmp->data, tmp->len);
				g_array_free(tmp, TRUE);
			default:
				break;
		}
	next:
		g_free(fname);
	}
	if(errno) {
		g_set_error(e, NBDS_ERR, NBDS_ERR_CFILE_READDIR_ERR, "Error trying to read directory: %s", strerror(errno));
	err_out:
		if(retval)
			g_array_free(retval, TRUE);
		return NULL;
	}
	return retval;
}
