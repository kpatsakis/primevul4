kern_path_mountpoint(int dfd, const char *name, struct path *path,
			unsigned int flags)
{
	struct filename s = {.name = name};
	return filename_mountpoint(dfd, &s, path, flags);
}
