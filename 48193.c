int ecryptfs_read_xattr_region(char *page_virt, struct inode *ecryptfs_inode)
{
	struct dentry *lower_dentry =
		ecryptfs_inode_to_private(ecryptfs_inode)->lower_file->f_dentry;
	ssize_t size;
	int rc = 0;

	size = ecryptfs_getxattr_lower(lower_dentry, ECRYPTFS_XATTR_NAME,
				       page_virt, ECRYPTFS_DEFAULT_EXTENT_SIZE);
	if (size < 0) {
		if (unlikely(ecryptfs_verbosity > 0))
			printk(KERN_INFO "Error attempting to read the [%s] "
			       "xattr from the lower file; return value = "
			       "[%zd]\n", ECRYPTFS_XATTR_NAME, size);
		rc = -EINVAL;
		goto out;
	}
out:
	return rc;
}
