process_mkdir(u_int32_t id)
{
	Attrib a;
	char *name;
	int r, mode, status = SSH2_FX_FAILURE;

	if ((r = sshbuf_get_cstring(iqueue, &name, NULL)) != 0 ||
	    (r = decode_attrib(iqueue, &a)) != 0)
		fatal("%s: buffer error: %s", __func__, ssh_err(r));

	mode = (a.flags & SSH2_FILEXFER_ATTR_PERMISSIONS) ?
	    a.perm & 07777 : 0777;
	debug3("request %u: mkdir", id);
	logit("mkdir name \"%s\" mode 0%o", name, mode);
	r = mkdir(name, mode);
	status = (r == -1) ? errno_to_portable(errno) : SSH2_FX_OK;
	send_status(id, status);
	free(name);
}
