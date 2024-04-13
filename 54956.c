process_extended_fstatvfs(u_int32_t id)
{
	int r, handle, fd;
	struct statvfs st;

	if ((r = get_handle(iqueue, &handle)) != 0)
		fatal("%s: buffer error: %s", __func__, ssh_err(r));
	debug("request %u: fstatvfs \"%s\" (handle %u)",
	    id, handle_to_name(handle), handle);
	if ((fd = handle_to_fd(handle)) < 0) {
		send_status(id, SSH2_FX_FAILURE);
		return;
	}
	if (fstatvfs(fd, &st) != 0)
		send_status(id, errno_to_portable(errno));
	else
		send_statvfs(id, &st);
}
