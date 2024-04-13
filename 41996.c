shmem_mknod(struct inode *dir, struct dentry *dentry, umode_t mode, dev_t dev)
{
	struct inode *inode;
	int error = -ENOSPC;

	inode = shmem_get_inode(dir->i_sb, dir, mode, dev, VM_NORESERVE);
	if (inode) {
		error = security_inode_init_security(inode, dir,
						     &dentry->d_name,
						     shmem_initxattrs, NULL);
		if (error) {
			if (error != -EOPNOTSUPP) {
				iput(inode);
				return error;
			}
		}
#ifdef CONFIG_TMPFS_POSIX_ACL
		error = generic_acl_init(inode, dir);
		if (error) {
			iput(inode);
			return error;
		}
#else
		error = 0;
#endif
		dir->i_size += BOGO_DIRENT_SIZE;
		dir->i_ctime = dir->i_mtime = CURRENT_TIME;
		d_instantiate(dentry, inode);
		dget(dentry); /* Extra count - pin the dentry in core */
	}
	return error;
}
