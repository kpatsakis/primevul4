void path_get(const struct path *path)
{
	mntget(path->mnt);
	dget(path->dentry);
}
