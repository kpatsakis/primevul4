static int evm_find_protected_xattrs(struct dentry *dentry)
{
	struct inode *inode = d_backing_inode(dentry);
	char **xattr;
	int error;
	int count = 0;

	if (!inode->i_op->getxattr)
		return -EOPNOTSUPP;

	for (xattr = evm_config_xattrnames; *xattr != NULL; xattr++) {
		error = inode->i_op->getxattr(dentry, *xattr, NULL, 0);
		if (error < 0) {
			if (error == -ENODATA)
				continue;
			return error;
		}
		count++;
	}

	return count;
}
