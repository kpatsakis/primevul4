process_fsetstat(u_int32_t id)
{
	Attrib a;
	int handle, fd, r;
	int status = SSH2_FX_OK;

	if ((r = get_handle(iqueue, &handle)) != 0 ||
	    (r = decode_attrib(iqueue, &a)) != 0)
		fatal("%s: buffer error: %s", __func__, ssh_err(r));

	debug("request %u: fsetstat handle %d", id, handle);
	fd = handle_to_fd(handle);
	if (fd < 0)
		status = SSH2_FX_FAILURE;
	else {
		char *name = handle_to_name(handle);

		if (a.flags & SSH2_FILEXFER_ATTR_SIZE) {
			logit("set \"%s\" size %llu",
			    name, (unsigned long long)a.size);
			r = ftruncate(fd, a.size);
			if (r == -1)
				status = errno_to_portable(errno);
		}
		if (a.flags & SSH2_FILEXFER_ATTR_PERMISSIONS) {
			logit("set \"%s\" mode %04o", name, a.perm);
			r = fchmod(fd, a.perm & 07777);
			if (r == -1)
				status = errno_to_portable(errno);
		}
		if (a.flags & SSH2_FILEXFER_ATTR_ACMODTIME) {
			char buf[64];
			time_t t = a.mtime;

			strftime(buf, sizeof(buf), "%Y%m%d-%H:%M:%S",
			    localtime(&t));
			logit("set \"%s\" modtime %s", name, buf);
			r = futimes(fd, attrib_to_tv(&a));
			if (r == -1)
				status = errno_to_portable(errno);
		}
		if (a.flags & SSH2_FILEXFER_ATTR_UIDGID) {
			logit("set \"%s\" owner %lu group %lu", name,
			    (u_long)a.uid, (u_long)a.gid);
			r = fchown(fd, a.uid, a.gid);
			if (r == -1)
				status = errno_to_portable(errno);
		}
	}
	send_status(id, status);
}
