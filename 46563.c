user_path_mountpoint_at(int dfd, const char __user *name, unsigned int flags,
			struct path *path)
{
	return filename_mountpoint(dfd, getname(name), path, flags);
}
