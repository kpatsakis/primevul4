process_fstat(u_int32_t id)
{
	Attrib a;
	struct stat st;
	int fd, r, handle, status = SSH2_FX_FAILURE;

	if ((r = get_handle(iqueue, &handle)) != 0)
		fatal("%s: buffer error: %s", __func__, ssh_err(r));
	debug("request %u: fstat \"%s\" (handle %u)",
	    id, handle_to_name(handle), handle);
	fd = handle_to_fd(handle);
	if (fd >= 0) {
		r = fstat(fd, &st);
		if (r < 0) {
			status = errno_to_portable(errno);
		} else {
			stat_to_attrib(&st, &a);
			send_attrib(id, &a);
			status = SSH2_FX_OK;
		}
	}
	if (status != SSH2_FX_OK)
		send_status(id, status);
}
