int kern_path(const char *name, unsigned int flags, struct path *path)
{
	struct nameidata nd;
	struct filename *filename = getname_kernel(name);
	int res = PTR_ERR(filename);

	if (!IS_ERR(filename)) {
		res = filename_lookup(AT_FDCWD, filename, flags, &nd);
		putname(filename);
		if (!res)
			*path = nd.path;
	}
	return res;
}
