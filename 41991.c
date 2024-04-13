static ssize_t shmem_listxattr(struct dentry *dentry, char *buffer, size_t size)
{
	struct shmem_inode_info *info = SHMEM_I(dentry->d_inode);
	return simple_xattr_list(&info->xattrs, buffer, size);
}
