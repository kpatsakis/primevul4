process_readlink(u_int32_t id)
{
	int r, len;
	char buf[PATH_MAX];
	char *path;

	if ((r = sshbuf_get_cstring(iqueue, &path, NULL)) != 0)
		fatal("%s: buffer error: %s", __func__, ssh_err(r));

	debug3("request %u: readlink", id);
	verbose("readlink \"%s\"", path);
	if ((len = readlink(path, buf, sizeof(buf) - 1)) == -1)
		send_status(id, errno_to_portable(errno));
	else {
		Stat s;

		buf[len] = '\0';
		attrib_clear(&s.attrib);
		s.name = s.long_name = buf;
		send_names(id, 1, &s);
	}
	free(path);
}
