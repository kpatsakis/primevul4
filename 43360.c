user_path_mountpoint_at(int dfd, const char __user *name, unsigned int flags,
			struct path *path)
{
	struct filename *s = getname(name);
	int error;
	if (IS_ERR(s))
		return PTR_ERR(s);
	error = filename_mountpoint(dfd, s, path, flags);
	putname(s);
	return error;
}
