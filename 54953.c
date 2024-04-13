process_close(u_int32_t id)
{
	int r, handle, ret, status = SSH2_FX_FAILURE;

	if ((r = get_handle(iqueue, &handle)) != 0)
		fatal("%s: buffer error: %s", __func__, ssh_err(r));

	debug3("request %u: close handle %u", id, handle);
	handle_log_close(handle, NULL);
	ret = handle_close(handle);
	status = (ret == -1) ? errno_to_portable(errno) : SSH2_FX_OK;
	send_status(id, status);
}
