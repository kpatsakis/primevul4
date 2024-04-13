void path_put(const struct path *path)
{
	dput(path->dentry);
	mntput(path->mnt);
}
