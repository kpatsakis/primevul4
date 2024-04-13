static int show_path_truncated(FILE *out, const struct name_path *path)
{
	int emitted, ours;

	if (!path)
		return 0;
	emitted = show_path_truncated(out, path->up);
	if (emitted < 0)
		return emitted;
	if (emitted)
		fputc('/', out);
	ours = show_path_component_truncated(out, path->elem, path->elem_len);
	if (ours < 0)
		return ours;
	return ours || emitted;
}
