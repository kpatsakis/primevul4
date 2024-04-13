static int sockfs_xattr_get(const struct xattr_handler *handler,
			    struct dentry *dentry, struct inode *inode,
			    const char *suffix, void *value, size_t size)
{
	if (value) {
		if (dentry->d_name.len + 1 > size)
			return -ERANGE;
		memcpy(value, dentry->d_name.name, dentry->d_name.len + 1);
	}
	return dentry->d_name.len + 1;
}
