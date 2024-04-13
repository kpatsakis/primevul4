process_setstat(u_int32_t id)
{
	Attrib a;
	char *name;
	int r, status = SSH2_FX_OK;

	if ((r = sshbuf_get_cstring(iqueue, &name, NULL)) != 0 ||
	    (r = decode_attrib(iqueue, &a)) != 0)
		fatal("%s: buffer error: %s", __func__, ssh_err(r));

	debug("request %u: setstat name \"%s\"", id, name);
	if (a.flags & SSH2_FILEXFER_ATTR_SIZE) {
		logit("set \"%s\" size %llu",
		    name, (unsigned long long)a.size);
		r = truncate(name, a.size);
		if (r == -1)
			status = errno_to_portable(errno);
	}
	if (a.flags & SSH2_FILEXFER_ATTR_PERMISSIONS) {
		logit("set \"%s\" mode %04o", name, a.perm);
		r = chmod(name, a.perm & 07777);
		if (r == -1)
			status = errno_to_portable(errno);
	}
	if (a.flags & SSH2_FILEXFER_ATTR_ACMODTIME) {
		char buf[64];
		time_t t = a.mtime;

		strftime(buf, sizeof(buf), "%Y%m%d-%H:%M:%S",
		    localtime(&t));
		logit("set \"%s\" modtime %s", name, buf);
		r = utimes(name, attrib_to_tv(&a));
		if (r == -1)
			status = errno_to_portable(errno);
	}
	if (a.flags & SSH2_FILEXFER_ATTR_UIDGID) {
		logit("set \"%s\" owner %lu group %lu", name,
		    (u_long)a.uid, (u_long)a.gid);
		r = chown(name, a.uid, a.gid);
		if (r == -1)
			status = errno_to_portable(errno);
	}
	send_status(id, status);
	free(name);
}
