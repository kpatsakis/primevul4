ssize_t ovl_getxattr(struct dentry *dentry, const char *name,
		     void *value, size_t size)
{
	struct path realpath;
	enum ovl_path_type type = ovl_path_real(dentry, &realpath);

	if (ovl_need_xattr_filter(dentry, type) && ovl_is_private_xattr(name))
		return -ENODATA;

	return vfs_getxattr(realpath.dentry, name, value, size);
}
