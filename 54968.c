process_rename(u_int32_t id)
{
	char *oldpath, *newpath;
	int r, status;
	struct stat sb;

	if ((r = sshbuf_get_cstring(iqueue, &oldpath, NULL)) != 0 ||
	    (r = sshbuf_get_cstring(iqueue, &newpath, NULL)) != 0)
		fatal("%s: buffer error: %s", __func__, ssh_err(r));

	debug3("request %u: rename", id);
	logit("rename old \"%s\" new \"%s\"", oldpath, newpath);
	status = SSH2_FX_FAILURE;
	if (lstat(oldpath, &sb) == -1)
		status = errno_to_portable(errno);
	else if (S_ISREG(sb.st_mode)) {
		/* Race-free rename of regular files */
		if (link(oldpath, newpath) == -1) {
			if (errno == EOPNOTSUPP) {
				struct stat st;

				/*
				 * fs doesn't support links, so fall back to
				 * stat+rename.  This is racy.
				 */
				if (stat(newpath, &st) == -1) {
					if (rename(oldpath, newpath) == -1)
						status =
						    errno_to_portable(errno);
					else
						status = SSH2_FX_OK;
				}
			} else {
				status = errno_to_portable(errno);
			}
		} else if (unlink(oldpath) == -1) {
			status = errno_to_portable(errno);
			/* clean spare link */
			unlink(newpath);
		} else
			status = SSH2_FX_OK;
	} else if (stat(newpath, &sb) == -1) {
		if (rename(oldpath, newpath) == -1)
			status = errno_to_portable(errno);
		else
			status = SSH2_FX_OK;
	}
	send_status(id, status);
	free(oldpath);
	free(newpath);
}
