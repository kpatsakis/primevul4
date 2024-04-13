process_write(u_int32_t id)
{
	u_int64_t off;
	size_t len;
	int r, handle, fd, ret, status;
	u_char *data;

	if ((r = get_handle(iqueue, &handle)) != 0 ||
	    (r = sshbuf_get_u64(iqueue, &off)) != 0 ||
	    (r = sshbuf_get_string(iqueue, &data, &len)) != 0)
		fatal("%s: buffer error: %s", __func__, ssh_err(r));

	debug("request %u: write \"%s\" (handle %d) off %llu len %zu",
	    id, handle_to_name(handle), handle, (unsigned long long)off, len);
	fd = handle_to_fd(handle);

	if (fd < 0)
		status = SSH2_FX_FAILURE;
	else {
		if (!(handle_to_flags(handle) & O_APPEND) &&
				lseek(fd, off, SEEK_SET) < 0) {
			status = errno_to_portable(errno);
			error("process_write: seek failed");
		} else {
/* XXX ATOMICIO ? */
			ret = write(fd, data, len);
			if (ret < 0) {
				error("process_write: write failed");
				status = errno_to_portable(errno);
			} else if ((size_t)ret == len) {
				status = SSH2_FX_OK;
				handle_update_write(handle, ret);
			} else {
				debug2("nothing at all written");
				status = SSH2_FX_FAILURE;
			}
		}
	}
	send_status(id, status);
	free(data);
}
