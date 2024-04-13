process_realpath(u_int32_t id)
{
	char resolvedname[PATH_MAX];
	char *path;
	int r;

	if ((r = sshbuf_get_cstring(iqueue, &path, NULL)) != 0)
		fatal("%s: buffer error: %s", __func__, ssh_err(r));

	if (path[0] == '\0') {
		free(path);
		path = xstrdup(".");
	}
	debug3("request %u: realpath", id);
	verbose("realpath \"%s\"", path);
	if (realpath(path, resolvedname) == NULL) {
		send_status(id, errno_to_portable(errno));
	} else {
		Stat s;
		attrib_clear(&s.attrib);
		s.name = s.long_name = resolvedname;
		send_names(id, 1, &s);
	}
	free(path);
}
