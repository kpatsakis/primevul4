static int shmem_setattr(struct dentry *dentry, struct iattr *attr)
{
	struct inode *inode = dentry->d_inode;
	int error;

	error = inode_change_ok(inode, attr);
	if (error)
		return error;

	if (S_ISREG(inode->i_mode) && (attr->ia_valid & ATTR_SIZE)) {
		loff_t oldsize = inode->i_size;
		loff_t newsize = attr->ia_size;

		if (newsize != oldsize) {
			i_size_write(inode, newsize);
			inode->i_ctime = inode->i_mtime = CURRENT_TIME;
		}
		if (newsize < oldsize) {
			loff_t holebegin = round_up(newsize, PAGE_SIZE);
			unmap_mapping_range(inode->i_mapping, holebegin, 0, 1);
			shmem_truncate_range(inode, newsize, (loff_t)-1);
			/* unmap again to remove racily COWed private pages */
			unmap_mapping_range(inode->i_mapping, holebegin, 0, 1);
		}
	}

	setattr_copy(inode, attr);
#ifdef CONFIG_TMPFS_POSIX_ACL
	if (attr->ia_valid & ATTR_MODE)
		error = generic_acl_chmod(inode);
#endif
	return error;
}
