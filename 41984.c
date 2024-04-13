static ssize_t shmem_getxattr(struct dentry *dentry, const char *name,
			      void *buffer, size_t size)
{
	struct shmem_inode_info *info = SHMEM_I(dentry->d_inode);
	int err;

	/*
	 * If this is a request for a synthetic attribute in the system.*
	 * namespace use the generic infrastructure to resolve a handler
	 * for it via sb->s_xattr.
	 */
	if (!strncmp(name, XATTR_SYSTEM_PREFIX, XATTR_SYSTEM_PREFIX_LEN))
		return generic_getxattr(dentry, name, buffer, size);

	err = shmem_xattr_validate(name);
	if (err)
		return err;

	return simple_xattr_get(&info->xattrs, name, buffer, size);
}
