struct dentry *user_path_create(int dfd, const char __user *pathname,
				struct path *path, unsigned int lookup_flags)
{
	struct filename *tmp = getname(pathname);
	struct dentry *res;
	if (IS_ERR(tmp))
		return ERR_CAST(tmp);
	res = filename_create(dfd, tmp, path, lookup_flags);
	putname(tmp);
	return res;
}
