void path_put(struct path *path)
{
	dput(path->dentry);
	mntput(path->mnt);
}
