static int sockfs_security_xattr_set(const struct xattr_handler *handler,
				     struct dentry *dentry, struct inode *inode,
				     const char *suffix, const void *value,
				     size_t size, int flags)
{
	/* Handled by LSM. */
	return -EAGAIN;
}
