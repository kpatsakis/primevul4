int inode_permission(struct inode *inode, int mask)
{
	int retval;

	if (mask & MAY_WRITE) {
		umode_t mode = inode->i_mode;

		/*
		 * Nobody gets write access to a read-only fs.
		 */
		if (IS_RDONLY(inode) &&
		    (S_ISREG(mode) || S_ISDIR(mode) || S_ISLNK(mode)))
			return -EROFS;

		/*
		 * Nobody gets write access to an immutable file.
		 */
		if (IS_IMMUTABLE(inode))
			return -EACCES;
	}

	if (inode->i_op->permission)
		retval = inode->i_op->permission(inode, mask);
	else
		retval = generic_permission(inode, mask, inode->i_op->check_acl);

	if (retval)
		return retval;

	retval = devcgroup_inode_permission(inode, mask);
	if (retval)
		return retval;

	return security_inode_permission(inode,
			mask & (MAY_READ|MAY_WRITE|MAY_EXEC|MAY_APPEND));
}
