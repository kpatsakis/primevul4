process_opendir(u_int32_t id)
{
	DIR *dirp = NULL;
	char *path;
	int r, handle, status = SSH2_FX_FAILURE;

	if ((r = sshbuf_get_cstring(iqueue, &path, NULL)) != 0)
		fatal("%s: buffer error: %s", __func__, ssh_err(r));

	debug3("request %u: opendir", id);
	logit("opendir \"%s\"", path);
	dirp = opendir(path);
	if (dirp == NULL) {
		status = errno_to_portable(errno);
	} else {
		handle = handle_new(HANDLE_DIR, path, 0, 0, dirp);
		if (handle < 0) {
			closedir(dirp);
		} else {
			send_handle(id, handle);
			status = SSH2_FX_OK;
		}

	}
	if (status != SSH2_FX_OK)
		send_status(id, status);
	free(path);
}
