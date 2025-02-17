handle_annotation(struct magic_set *ms, struct magic *m)
{
	if (ms->flags & MAGIC_APPLE) {
		if (file_printf(ms, "%.8s", m->apple) == -1)
			return -1;
		return 1;
	}
	if ((ms->flags & MAGIC_MIME_TYPE) && m->mimetype[0]) {
		if (file_printf(ms, "%s", m->mimetype) == -1)
			return -1;
		return 1;
	}
	return 0;
}
