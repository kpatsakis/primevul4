int generic_permission(struct inode *inode, int mask)
{
	int ret;

	/*
	 * Do the basic permission checks.
	 */
	ret = acl_permission_check(inode, mask);
	if (ret != -EACCES)
		return ret;

	if (S_ISDIR(inode->i_mode)) {
		/* DACs are overridable for directories */
		if (!(mask & MAY_WRITE))
			if (capable_wrt_inode_uidgid(inode,
						     CAP_DAC_READ_SEARCH))
				return 0;
		if (capable_wrt_inode_uidgid(inode, CAP_DAC_OVERRIDE))
			return 0;
		return -EACCES;
	}

	/*
	 * Searching includes executable on directories, else just read.
	 */
	mask &= MAY_READ | MAY_WRITE | MAY_EXEC;
	if (mask == MAY_READ)
		if (capable_wrt_inode_uidgid(inode, CAP_DAC_READ_SEARCH))
			return 0;
	/*
	 * Read/write DACs are always overridable.
	 * Executable DACs are overridable when there is
	 * at least one exec bit set.
	 */
	if (!(mask & MAY_EXEC) || (inode->i_mode & S_IXUGO))
		if (capable_wrt_inode_uidgid(inode, CAP_DAC_OVERRIDE))
			return 0;

	return -EACCES;
}
