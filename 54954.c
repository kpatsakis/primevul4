process_do_stat(u_int32_t id, int do_lstat)
{
	Attrib a;
	struct stat st;
	char *name;
	int r, status = SSH2_FX_FAILURE;

	if ((r = sshbuf_get_cstring(iqueue, &name, NULL)) != 0)
		fatal("%s: buffer error: %s", __func__, ssh_err(r));

	debug3("request %u: %sstat", id, do_lstat ? "l" : "");
	verbose("%sstat name \"%s\"", do_lstat ? "l" : "", name);
	r = do_lstat ? lstat(name, &st) : stat(name, &st);
	if (r < 0) {
		status = errno_to_portable(errno);
	} else {
		stat_to_attrib(&st, &a);
		send_attrib(id, &a);
		status = SSH2_FX_OK;
	}
	if (status != SSH2_FX_OK)
		send_status(id, status);
	free(name);
}
