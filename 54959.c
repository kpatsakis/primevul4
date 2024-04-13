process_extended_statvfs(u_int32_t id)
{
	char *path;
	struct statvfs st;
	int r;

	if ((r = sshbuf_get_cstring(iqueue, &path, NULL)) != 0)
		fatal("%s: buffer error: %s", __func__, ssh_err(r));
	debug3("request %u: statvfs", id);
	logit("statvfs \"%s\"", path);

	if (statvfs(path, &st) != 0)
		send_status(id, errno_to_portable(errno));
	else
		send_statvfs(id, &st);
        free(path);
}
