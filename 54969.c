process_rmdir(u_int32_t id)
{
	char *name;
	int r, status;

	if ((r = sshbuf_get_cstring(iqueue, &name, NULL)) != 0)
		fatal("%s: buffer error: %s", __func__, ssh_err(r));

	debug3("request %u: rmdir", id);
	logit("rmdir name \"%s\"", name);
	r = rmdir(name);
	status = (r == -1) ? errno_to_portable(errno) : SSH2_FX_OK;
	send_status(id, status);
	free(name);
}
