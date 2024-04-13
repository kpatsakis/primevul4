struct dentry *kern_path_locked(const char *name, struct path *path)
{
	struct filename *filename = getname_kernel(name);
	struct nameidata nd;
	struct dentry *d;
	int err;

	if (IS_ERR(filename))
		return ERR_CAST(filename);

	err = filename_lookup(AT_FDCWD, filename, LOOKUP_PARENT, &nd);
	if (err) {
		d = ERR_PTR(err);
		goto out;
	}
	if (nd.last_type != LAST_NORM) {
		path_put(&nd.path);
		d = ERR_PTR(-EINVAL);
		goto out;
	}
	mutex_lock_nested(&nd.path.dentry->d_inode->i_mutex, I_MUTEX_PARENT);
	d = __lookup_hash(&nd.last, nd.path.dentry, 0);
	if (IS_ERR(d)) {
		mutex_unlock(&nd.path.dentry->d_inode->i_mutex);
		path_put(&nd.path);
		goto out;
	}
	*path = nd.path;
out:
	putname(filename);
	return d;
}
